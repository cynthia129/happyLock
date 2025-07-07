#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <array>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket);
    void start();

private:
    void do_read();
    void do_write(const std::string& msg);

    boost::asio::ip::tcp::socket socket_;
    std::array<char, 1024> buffer_;
}; 