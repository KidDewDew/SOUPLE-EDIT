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
            std::this_thread::sleep_for(std::chrono::seconds(3));
            task.succeeded = true;
            if(task.succeeded) {
                std::cout << "[info]Task " << task.task_id << " Finished." << std::endl;
            } else {
                std::cout << "[error]Task " << task.task_id << " Failed: "
                          << task.failed_reason << std::endl;
            }
            task.generated_filepath = task.pdf_filepath;
            task_notifier(task); //通知

            //删除pdf文件
            if(! std::filesystem::remove(task.pdf_filepath)) {
                std::cout << "[error]Remove PDF file failed.\n";
            }
        }
    }
private:
    static inline std::mutex mutex_for_queue;
    static inline std::condition_variable task_cv;
    static inline std::vector<std::thread> _threads; //线程
    static inline std::queue<Task> _task_queue;
    static inline std::function<void(const Task&)> task_notifier;
};

#endif // WORKER_H
