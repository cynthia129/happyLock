#pragma once
#include <boost/asio.hpp>
#include <memory>
#include "session.h"
#include "../service/CollaborationService.h"
#include <mutex>

// Server类负责启动TCP服务器，接受客户端连接，并为每个连接创建Session对象。
class Server {
public:
    // 构造函数，初始化监听端口
    Server(boost::asio::io_context& io_context, short port);

private:
    // 异步接受新连接
    void do_accept();
    boost::asio::ip::tcp::acceptor acceptor_; // TCP连接接受器
    CollaborationService collabService_;      // 协同服务对象，管理所有Session
};

// 全局连接数互斥锁和计数器
extern std::mutex g_conn_mutex; // 保护g_conn_count的互斥锁
extern int g_conn_count;        // 当前活跃连接数 