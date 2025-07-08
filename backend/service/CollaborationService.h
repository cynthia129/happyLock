#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <set>
#include <functional>

class Session; // 前向声明，表示会话对象

// 协同算法接口，支持不同的协同编辑算法集成（如OT、diff-match-patch等）
class ICollabAlgorithm {
public:
    virtual ~ICollabAlgorithm() = default;
    // 应用协同操作，返回操作后的内容
    virtual std::string applyOperation(const std::string& base, const std::string& op) = 0;
};

// CollaborationService类负责管理文档的所有在线会话，支持消息广播和用户列表维护。
class CollaborationService {
public:
    using BroadcastFunc = std::function<void(const std::string& msg, std::shared_ptr<Session> except)>;

    // 构造函数
    CollaborationService();
    // 添加会话到指定文档
    void addSession(int docId, std::shared_ptr<Session> session);
    // 从指定文档移除会话
    void removeSession(int docId, std::shared_ptr<Session> session);
    // 向指定文档的所有会话广播消息，except为可选排除对象
    void broadcast(int docId, const std::string& msg, std::shared_ptr<Session> except = nullptr);

    // 设置协同算法实现
    void setAlgorithm(std::shared_ptr<ICollabAlgorithm> algo);
    // 应用协同操作
    std::string applyOperation(const std::string& base, const std::string& op);

    // 获取指定文档的所有在线用户名
    std::vector<std::string> getOnlineUsers(int docId);
    // 广播当前文档的用户列表
    void broadcastUserList(int docId);

private:
    std::mutex mtx_; // 互斥锁，保护会话集合
    // 每个文档id对应一组Session
    std::unordered_map<int, std::set<std::shared_ptr<Session>>> docSessions_;
    std::shared_ptr<ICollabAlgorithm> algorithm_; // 当前协同算法实现
}; 