#pragma once
#include <boost/asio.hpp>
#include <memory>
#include "session.h"
#include "../service/CollaborationService.h"
#include <mutex>

class Server {
public:
    Server(boost::asio::io_context& io_context, short port);

private:
    void do_accept();
    boost::asio::ip::tcp::acceptor acceptor_;
    CollaborationService collabService_;
};

extern std::mutex g_conn_mutex;
extern int g_conn_count; 