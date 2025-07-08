#include "server.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <sstream>

// 全局连接数互斥锁和计数器定义
std::mutex g_conn_mutex;
int g_conn_count = 0;

namespace {
// 获取当前时间字符串，格式：YYYY-MM-DD HH:MM:SS
std::string now_time() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
// 彩色输出辅助函数
std::string green(const std::string& s) { return s; }
std::string yellow(const std::string& s) { return s; }
std::string red(const std::string& s) { return s; }
}

// Server构造函数，初始化监听端口并启动异步接受
Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    std::cout << green("[Server]") << " [" << now_time() << "] 启动，监听端口: " << port << std::endl;
    do_accept();
}

// 异步接受新连接，创建Session并加入协同服务
void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                {
                    std::lock_guard<std::mutex> lock(g_conn_mutex);
                    ++g_conn_count;
                }
                std::cout << yellow("[Server]") << " [" << now_time() << "] 新客户端连接: " << socket.remote_endpoint()
                          << " | 当前连接数: " << g_conn_count << std::endl;
                auto session = std::make_shared<Session>(std::move(socket), &collabService_);
                collabService_.addSession(1, session); // 默认加入文档1
                session->start();
            } else {
                std::cerr << red("[Server]") << " [" << now_time() << "] 接受连接出错: " << ec.message() << std::endl;
            }
            do_accept();
        }
    );
} 