#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <json.h>
#include <fstream>
#include <iostream>
#include <chrono>
#include <format>
#include <random>
#include <filesystem>
#include "worker.h"
#include "helper_web.h"

/// PDF表格提取 分布服务程序
/// 启动该程序即可启动一个表格提取服务。

/// main.cpp包含简单的网络架构
/// 设计：读线程+写线程+工作线程池

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

std::string HOST = "127.0.0.1";
std::string PORT = "8080";
const std::string SERVICE_NAME = "PDFTableExtract";
const std::string PASSWORD = "1d2f3ghduwaijiajdiJAU82131fsdfjuji";

std::string this_service_id; //本进程分配得的服务ID

inline net::io_context& ioc() {
    static net::io_context _ioc;
    return _ioc;
}

inline auto& get_strand() {
    static auto strand = net::make_strand(ioc());
    return strand;
}

inline websocket::stream<tcp::socket>& ws() {
    static websocket::stream<tcp::socket> _ws(get_strand());
    return _ws;
}

std::mutex mutex_ws; //对websocket的互斥锁

void connect_to_server() {
    // 建立连接
    net::post(get_strand(), [&]() {
        try{
            tcp::resolver resolver(ioc());
            auto const results = resolver.resolve(HOST, PORT);
            net::connect(ws().next_layer(), results);

            // WebSocket握手
            std::string path = "/SouplePDF_Web/service-websocket/" + SERVICE_NAME + "/" + PASSWORD;
            ws().handshake(HOST + ":" + PORT, path);
            std::cout << "[info]Connected to WebSocket server" << std::endl;

            if(! ws().is_open()) {
                throw std::runtime_error("WebSocket is_open(): false");
            }

            // 接收初始消息
            beast::flat_buffer buffer;
            ws().read(buffer);

            Json::Value jv;
            Json::Reader().parse(beast::buffers_to_string(buffer.data()),jv);
            this_service_id = jv["service_id"].asString();
            std::cout << "[info]service-id:" << this_service_id << std::endl;
        } catch(const std::exception& e) {
            std::cerr << "[Exception]" << e.what() << std::endl;
            std::cout << "[info]sleep for 2secs.\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    });
}

void process_read_buffer(const beast::flat_buffer& buffer) {
    std::string data_str = beast::buffers_to_string(buffer.data());
    if(data_str.length() < 4) {
        std::cout << "[error]Error Message: length<4\n";
        return;
    }
    char* data = data_str.data();
    //大端序转换
    int json_length = (((int)data[0]) << 24) + (((int)data[1]) << 16)
              + (((int)data[2]) << 8) + ((int)data[3]);
    if(data_str.length() < 4+json_length) {
        std::cout << "[error]Error Message: 4+json_length=" << 4+json_length
                  << ",but the length of message is " << data_str.length() << std::endl;
        return;
    }
    Json::Value jv;
    if(! Json::Reader().parse(data+4,data+4+json_length,jv)) {
        std::cout << "[error]Parse json failed: " << std::string_view(data+4,json_length)
                  << std::endl;
        return;
    }
    if(jv.get("action","null").asString() == "extract") {
        //提取任务
        bool oneExcelFile = jv.get("oneExcelFile",false).asBool();
        std::string task_id = jv.get("task_id","").asString();
        if(task_id.empty()) {
            std::cout << "[error]Json failed: Key 'task_id' not found."
                      << std::endl;
            return;
        }
        //提取PDF文件
        std::string filename = std::format("pdf_wait_extract_table/{}.pdf",
                                    souple_web::generateUID());
        std::ofstream file(filename,std::ios_base::binary);
        file.write(data+4+json_length,data_str.length() - 4 - json_length);
        file.close();
        std::cout << "[new task] Task_id is" << task_id << ".Extract Table From: " << filename << std::endl;
        Worker::Task task;
        task.oneExcelFile = oneExcelFile;
        task.pdf_filepath = std::move(filename);
        task.task_id = std::move(task_id);
        Worker::addTask(task);
    }
}

//读循环
void read_loop() {
    auto buffer = std::make_shared<beast::flat_buffer>();
    // 接收消息
    try{
        ws().async_read(*buffer,[buffer](beast::error_code ec, std::size_t bytes){
            if(!ec) {
                try {
                    process_read_buffer(*buffer);
                }catch(const std::exception& e) {
                    std::cout << "[Exception]" << e.what() << std::endl;
                }
                read_loop();
            } else {
                std::cout << "[error]Read failed: " << ec.message() << std::endl;
                if(ws().is_open() == false) {
                    std::cout << "[info]Reconnecting...\n";
                    try{
                        connect_to_server();
                    }catch(std::exception& e) {
                        std::cout << "[Exception]" << e.what() << std::endl;
                    }
                }
                read_loop();
            }
        });
    } catch(const std::exception& e) {
        std::cout << "[Exception]" << e.what() << std::endl;
    }
}


std::queue<std::shared_ptr<std::string>> queue_write_data;
bool has_async_write = false; //是否有正在执行的async_write
// 写循环
void write_loop() {
    if(has_async_write == true) return; //已经有async_write
    if(queue_write_data.empty()) { //写队列为空
        return;
    }
    auto data_ptr = queue_write_data.front();
    queue_write_data.pop();
    has_async_write = true;
    try{
        ws().async_write(net::buffer(*data_ptr),
        [data_ptr](beast::error_code ec, std::size_t bytes){
            if(ec) {
                std::cout << "[error]Send failed: " << ec.message() << std::endl;
            } //else {
                //std::cout << "[info]Sent " << bytes << std::endl;
            //}
            has_async_write = false;
            write_loop(); //触发下一次写
        });
    } catch(const std::exception& e) {
        std::cout << "[Exception]" << e.what() << std::endl;
    }
}

void send_message(std::string data) {
    auto data_ptr = std::make_shared<std::string>(std::move(data));
    net::post(get_strand(),[data_ptr]{
        queue_write_data.push(data_ptr);
        write_loop();
    });
}

// 任务回调函数
void task_notify(const Worker::Task& task) {
    //send result
    Json::Value jv;
    jv["succeeded"] = task.isSucceeded();
    jv["task_id"] = task.task_id;
    jv["finished"] = true;
    jv["suffix"] = task.oneExcelFile ? ".xlsx" : ".zip";
    if(task.isSucceeded() == false) {
        jv["failed_reason"] = task.get_failed_reason().data();
        std::string data = souple_web::concatStrAndBinary(Json::FastWriter().write(jv),0,0);
        send_message(data);
    } else {
        std::ifstream is(task.get_generated_filepath().data(),std::ios::binary|std::ios::ate);
        if(!is.is_open()) {
            jv["succeeded"] = false;
            std::string data = souple_web::concatStrAndBinary(Json::FastWriter().write(jv),0,0);
            send_message(data);
            return;
        }
        std::streamsize size = is.tellg();
        is.seekg(0, std::ios::beg);

        char *buffer = new char[size];
        is.read(buffer,size);
        is.close();

        std::string data = souple_web::concatStrAndBinary(Json::FastWriter().write(jv),buffer,size);
        delete[] buffer;
        send_message(data);

        //发完即删
        if(!std::filesystem::remove(task.get_generated_filepath())) {
           std::cout << "[Error]std::filesystem::remove(task.get_generated_filepath()) failed.\n";
        }
    }
}

int main(int argc,char** argv)
{
    if(argc < 3) {
        std::cout << "[info]dafault server:127.0.0.1:8080\n";
    } else {
        HOST = argv[1];
        PORT = argv[2];
        std::cout << "[info]configured server:"<<HOST<<':'<<PORT<<std::endl;
    }
    //禁用qDebug、qInfo输出
    //qputenv("QT_LOGGING_RULES", "*.debug=false;*.info=false");
    //初始化一个gui-app对象，以便Qt库正常工作
    QGuiApplication app(argc,argv);

    std::cout << "--- Welcome to SoupleService-PDFTableExtract. ---\n";
    if(!std::filesystem::exists("./pdf_wait_extract_table")) {
        if(!std::filesystem::create_directories("./pdf_wait_extract_table")) {
            std::cout << "Create Dir 'pdf_wait_extract_table' Failed.\n";
            return 1;
        }
    }

    if(!std::filesystem::exists("./TEMP_generated")) {
        if(!std::filesystem::create_directories("./TEMP_generated")) {
            std::cout << "Create Dir 'TEMP_generated' Failed.\n";
            return 1;
        }
    }

    Helper::init(); //初始化helper

    //二进制mode
    ws().binary(true); //初始化网络和io
    ws().read_message_max(50 * 1024 * 1024); //最多50MB读取缓冲区

    connect_to_server();

    Worker::init(&task_notify);

    net::post(get_strand(),[]{
        read_loop();
        write_loop();
    });

    ioc().run();

    // 关闭连接 [实际不可达]
    ws().close(websocket::close_code::normal);
    std::cout << "Connection closed" << std::endl;

    return 0;
}
