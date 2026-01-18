#ifndef WORKER_H
#define WORKER_H
#include "config.h"
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <iostream>
#include <functional>
#include <filesystem>
#include <fstream>
#include "zip.h"
#include "helper_web.h"
#include "../pdf2souple.h"
#include "../table_extract_tool.h"
///本文件包含了工作线程池，供main.cpp使用

class Worker {
public:

    // 任务类
    struct Task {
        friend class Worker;
    public:
        //是否把提取得的表格存储在一个excel文件里(多工作表)
        bool oneExcelFile;
        //任务ID
        std::string task_id;
        //待提取表格的pdf文件路径
        std::string pdf_filepath;
        bool isSucceeded() const noexcept {
            return succeeded;
        }
        std::string_view get_failed_reason() const noexcept {
            return failed_reason;
        }
        std::string_view get_generated_filepath() const noexcept {
            return generated_filepath;
        }
    private:
        //任务是否成功完成
        bool succeeded;
        //如果succeeded==false，失败原因为
        std::string failed_reason;
        //产生的文件路径
        std::string generated_filepath;
    };
    static inline void init(std::function<void(const Task& task)> task_notifier) {
        FPDF_InitLibrary(); //初始化fpdf
        Worker::task_notifier = task_notifier;
        for(int i = 0; i < Config::WORKER_THREAD_NUM; ++i) {
            _threads.push_back(std::thread(&Worker::worker_thread_loop));
        }
        std::cout << "[info]Worker threads have been started.Number:" << Config::WORKER_THREAD_NUM << std::endl;
    }
    static inline void addTask(Task task) {
        std::lock_guard<std::mutex> lock(mutex_for_queue);
        _task_queue.push(std::move(task));
        task_cv.notify_one();
    }
private:
    static inline void worker_thread_loop() {
        SoupleManager::init_for_thread(); //初始化SoupleManager
        Pdf2Souple::init_for_thread();
        while(true) {
            std::unique_lock<std::mutex> lock(mutex_for_queue);
            // 等待任务队列有一个任务
            task_cv.wait(lock,[]{
                return !_task_queue.empty();
            });
            //该任务已经被本线程抢占
            Task task = std::move(_task_queue.front());
            _task_queue.pop();
            lock.unlock();
            std::cout << "[info]Thread " << std::this_thread::get_id()
                      << " is working on Task " << task.task_id << std::endl;

            //std::this_thread::sleep_for(std::chrono::seconds(3));

            /* 执行任务 */
            task.succeeded = doTask(task);

            SoupleManager::clear_for_thread(); //清理SoupleManager

            if(task.succeeded) {
                std::cout << "[info]Task " << task.task_id << " Finished." << std::endl;
            } else {
                std::cout << "[error]Task " << task.task_id << " Failed: "
                          << task.failed_reason << std::endl;
            }

            task_notifier(task); //通知

            //删除pdf文件
            if(! std::filesystem::remove(task.pdf_filepath)) {
                std::cout << "[error]Remove PDF file failed.\n";
            }
        }
    }

    //执行表格提取的任务
    static bool doTask(Task& task) noexcept {
        if(! Pdf2Souple::loadPDF_ofProTableExtract(QString(task.pdf_filepath.c_str())) )
        {
            std::cout << "[error]Analyse PDF failed\n";
            return false;
        }

        //遍历souple对象，寻找表格
        std::set<TableInfo*> tableinfos;//(注意去重)
        for(auto obj : SoupleManager::getDocumentObjs(SoupleManager::Current_Document))
        {
            std::cout << obj->__dstr().toStdString() << std::endl;
            auto tableline = obj->as<TableLine*>();
            if(tableline) {
                tableinfos.insert(tableline->getTableInfo());
            }
        }

        // 无表格
        if(tableinfos.empty()) {
            task.failed_reason = "未检测到表格。";
            return false;
        }

        std::cout << "[info]has " << tableinfos.size() << " tables.\n";

        std::string uid = souple_web::generateUID();
        task.generated_filepath = "./TEMP_generated/"+souple_web::generateUID();
        if(task.oneExcelFile) {
            task.generated_filepath += ".xlsx";
            //一个文件多个表，最终格式为.xlsx
            auto workbook = spt_new_workbook(task.generated_filepath.c_str());
            for(auto tableinfo : tableinfos) {
                spt_add_worksheet_of_table(workbook,NULL,tableinfo);
            }
            spt_free_workbook(workbook);
        } else {
            task.generated_filepath += ".zip";
            //一个文件一个表，最终压缩为.zip
            std::vector<std::string> xlsx_filename_list;
            for(auto tableinfo : tableinfos) {
                std::string workbook_filename =
                        "./TEMP_generated/"+souple_web::generateUID()+".xlsx";
                auto workbook = spt_new_workbook(workbook_filename.c_str());
                spt_add_worksheet_of_table(workbook,NULL,tableinfo);
                spt_free_workbook(workbook);
                xlsx_filename_list.push_back(std::move(workbook_filename));
            }
            zipFile zf = zipOpen(task.generated_filepath.c_str(),APPEND_STATUS_CREATE);
            char buf[10240]; //10kB
            char inner_filename[32];
            int file_id = 1;
            for(auto& file : xlsx_filename_list) {
                sprintf(inner_filename,"表格(%d).xlsx",file_id);
                int err = zipOpenNewFileInZip(zf, inner_filename, NULL, NULL, 0, NULL, 0,
                                              NULL, Z_DEFLATED, Z_BEST_COMPRESSION);
                if (err != ZIP_OK) {
                    std::cout << "[error]zip create failed.\n";
                    continue;
                }
                std::ifstream is(file,std::ios::binary);
                if(! is.is_open()) {
                    std::cout << "[error]open xlsx failed.\n";
                    continue; //跳过
                }
                int len;
                while(is.read(buf,10240)) {
                    if(is.gcount() == 0) break;
                    zipWriteInFileInZip(zf,buf,is.gcount());
                }
                is.close();
                zipCloseFileInZip(zf);
                ++ file_id;
            }
            zipClose(zf,NULL);
        }

        std::cout << "[info]generated: " << task.generated_filepath << std::endl;

        return true;
    }

private:
    static inline std::mutex mutex_for_queue;
    static inline std::condition_variable task_cv;
    static inline std::vector<std::thread> _threads; //线程
    static inline std::queue<Task> _task_queue;
    static inline std::function<void(const Task&)> task_notifier;
};

#endif // WORKER_H
