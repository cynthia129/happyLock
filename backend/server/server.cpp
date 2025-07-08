#include "server.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <sstream>

// ȫ���������������ͼ���������
std::mutex g_conn_mutex;
int g_conn_count = 0;

namespace {
// ��ȡ��ǰʱ���ַ�������ʽ��YYYY-MM-DD HH:MM:SS
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
// ��ɫ�����������
std::string green(const std::string& s) { return s; }
std::string yellow(const std::string& s) { return s; }
std::string red(const std::string& s) { return s; }
}

// Server���캯������ʼ�������˿ڲ������첽����
Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    std::cout << green("[Server]") << " [" << now_time() << "] �����������˿�: " << port << std::endl;
    do_accept();
}

// �첽���������ӣ�����Session������Эͬ����
void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                {
                    std::lock_guard<std::mutex> lock(g_conn_mutex);
                    ++g_conn_count;
                }
                std::cout << yellow("[Server]") << " [" << now_time() << "] �¿ͻ�������: " << socket.remote_endpoint()
                          << " | ��ǰ������: " << g_conn_count << std::endl;
                auto session = std::make_shared<Session>(std::move(socket), &collabService_);
                collabService_.addSession(1, session); // Ĭ�ϼ����ĵ�1
                session->start();
            } else {
                std::cerr << red("[Server]") << " [" << now_time() << "] �������ӳ���: " << ec.message() << std::endl;
            }
            do_accept();
        }
    );
} 