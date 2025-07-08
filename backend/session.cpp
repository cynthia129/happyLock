#include "session.h"
#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)) {}

void Session::start() {
    do_read();
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::string msg(buffer_.data(), length);
                try {
                    auto j = json::parse(msg);
                    std::string type = j.value("type", "");
                    if (type == "edit") {
                        std::string user = j.value("user", "");
                        std::string content = j.value("content", "");
                        int version = j.value("version", 0);
                        std::cout << "Edit from " << user << ": " << content << " (v" << version << ")" << std::endl;

                        // 构造回执消息
                        json resp;
                        resp["type"] = "ack";
                        resp["status"] = "ok";
                        resp["version"] = version;
                        do_write(resp.dump());
                    } else {
                        // 其他类型处理
                    }
                } catch (std::exception& e) {
                    std::cerr << "JSON parse error: " << e.what() << std::endl;
                }
                do_read();
            }
        });
}

void Session::do_write(const std::string& msg) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(msg),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            // 可选：处理写完成
        });
} 