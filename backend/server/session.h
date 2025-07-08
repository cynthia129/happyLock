#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <array>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class CollaborationService; // 前向声明

// Session类表示一个客户端会话，负责收发消息和与协同服务交互。
class Session : public std::enable_shared_from_this<Session> {
public:
    // 构造函数，初始化socket和协同服务指针
    Session(boost::asio::ip::tcp::socket socket, CollaborationService* collabService);
    // 析构函数，处理连接关闭
    ~Session();
    // 启动会话，开始异步读取
    void start();
    // 发送消息到客户端
    void do_write(const std::string& msg); // 公共接口

    int docId = 1;            // 当前会话所属文档ID
    std::string username;     // 当前会话用户名

private:
    // 异步读取客户端消息
    void do_read();

    boost::asio::ip::tcp::socket socket_;      // TCP连接socket
    std::array<char, 1024> buffer_;            // 读缓冲区
    CollaborationService* collabService_;      // 协同服务指针
    std::string read_buffer_;                  // 消息拼接缓冲
}; 