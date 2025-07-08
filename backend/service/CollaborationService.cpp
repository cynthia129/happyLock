#include "CollaborationService.h"
#include "../server/session.h"
#include <vector>
#include <nlohmann/json.hpp>

// 构造函数
CollaborationService::CollaborationService() {}

// 添加会话到指定文档的会话集合
void CollaborationService::addSession(int docId, std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mtx_);
    docSessions_[docId].insert(session);
}

// 从指定文档移除会话
void CollaborationService::removeSession(int docId, std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = docSessions_.find(docId);
    if (it != docSessions_.end()) {
        it->second.erase(session);
        if (it->second.empty()) {
            docSessions_.erase(it);
        }
    }
}

// 向指定文档的所有会话广播消息，except为可选排除对象
void CollaborationService::broadcast(int docId, const std::string& msg, std::shared_ptr<Session> except) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = docSessions_.find(docId);
    if (it != docSessions_.end()) {
        for (auto& session : it->second) {
            if (session != except) {
                session->do_write(msg); // 需要Session的do_write为public
            }
        }
    }
}

// 设置协同算法实现
void CollaborationService::setAlgorithm(std::shared_ptr<ICollabAlgorithm> algo) {
    algorithm_ = algo;
}

// 应用协同操作，返回操作后的内容
std::string CollaborationService::applyOperation(const std::string& base, const std::string& op) {
    if (algorithm_) {
        return algorithm_->applyOperation(base, op);
    }
    return base;
}

// 获取指定文档的所有在线用户名
std::vector<std::string> CollaborationService::getOnlineUsers(int docId) {
    std::vector<std::string> users;
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = docSessions_.find(docId);
    if (it != docSessions_.end()) {
        for (auto& session : it->second) {
            if (session && !session->username.empty())
                users.push_back(session->username);
        }
    }
    return users;
}

// 广播当前文档的用户列表
void CollaborationService::broadcastUserList(int docId) {
    auto users = getOnlineUsers(docId);
    nlohmann::json msg = {
        {"type", "user_list"},
        {"docId", docId},
        {"users", users}
    };
    auto it = docSessions_.find(docId);
    if (it != docSessions_.end()) {
        for (auto& session : it->second) {
            if (session)
                session->do_write(msg.dump());
        }
    }
} 