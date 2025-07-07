#pragma once
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <set>
#include <functional>

class Session; // 前向声明

// 协同算法接口（可集成libot/diff-match-patch等）
class ICollabAlgorithm {
public:
    virtual ~ICollabAlgorithm() = default;
    virtual std::string applyOperation(const std::string& base, const std::string& op) = 0;
};

// 多客户端广播与协同服务
class CollaborationService {
public:
    using BroadcastFunc = std::function<void(const std::string& msg, std::shared_ptr<Session> except)>;

    CollaborationService();
    void addSession(int docId, std::shared_ptr<Session> session);
    void removeSession(int docId, std::shared_ptr<Session> session);
    void broadcast(int docId, const std::string& msg, std::shared_ptr<Session> except = nullptr);

    void setAlgorithm(std::shared_ptr<ICollabAlgorithm> algo);
    std::string applyOperation(const std::string& base, const std::string& op);

    std::vector<std::string> getOnlineUsers(int docId);
    void broadcastUserList(int docId);

private:
    std::mutex mtx_;
    // 每个文档id对应一组Session
    std::unordered_map<int, std::set<std::shared_ptr<Session>>> docSessions_;
    std::shared_ptr<ICollabAlgorithm> algorithm_;
}; 