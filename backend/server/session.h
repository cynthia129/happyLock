#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <array>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class CollaborationService;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, CollaborationService* collabService);
    ~Session();
    void start();
    void do_write(const std::string& msg); // 公开

    int docId = 1;
    std::string username;

private:
    void do_read();

    boost::asio::ip::tcp::socket socket_;
    std::array<char, 1024> buffer_;
    CollaborationService* collabService_;
    std::string read_buffer_;
}; 