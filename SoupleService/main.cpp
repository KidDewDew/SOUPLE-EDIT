//#include <QCoreApplication>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <stdio.h>
#include <iostream>
//#include "../obj.h"  //引入obj.h

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace http = beast::http;

using tcp = boost::asio::ip::tcp;


// SoupleService: SoupleEdit的附属软件，用来作为后端服务。
// SoupleService运行在Windows端，需要配置主服务器。
int main(int argc, char *argv[])
{
    printf("Welcome to souple-service Software.\n"
           "Connecting to service...");
    try {
        net::io_context ioc;

                // 使用免费的时间API
                std::string host = "www.baidu.com";
                std::string target = "/loading.html"; // 获取上海时间

                // 创建TCP解析器和流
                tcp::resolver resolver(ioc);
                beast::tcp_stream stream(ioc);

                // 解析主机名
                auto const results = resolver.resolve(host, "http");

                // 连接到服务器
                stream.connect(results);

                // 创建HTTP GET请求
                http::request<http::string_body> req{http::verb::get, target, 11};
                req.set(http::field::host, host);
                req.set(http::field::user_agent, "Boost Beast Client");

                // 发送HTTP请求
                http::write(stream, req);

                // 接收响应
                beast::flat_buffer buffer;
                http::response<http::dynamic_body> res;
                http::read(stream, buffer, res);

                // 输出响应体（JSON格式的时间信息）
                std::string response_body = beast::buffers_to_string(res.body().data());
                std::cout << "Current time from API:\n" << response_body << std::endl;

                // 优雅关闭连接
                beast::error_code ec;
                stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
