#include "CollaborationService.h"
#include "../server/session.h"
#include <vector>
#include <nlohmann/json.hpp>

CollaborationService::CollaborationService() {}

void CollaborationService::addSession(int docId, std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mtx_);
    docSessions_[docId].insert(session);
}

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

void CollaborationService::setAlgorithm(std::shared_ptr<ICollabAlgorithm> algo) {
    algorithm_ = algo;
}

std::string CollaborationService::applyOperation(const std::string& base, const std::string& op) {
    if (algorithm_) {
        return algorithm_->applyOperation(base, op);
    }
    return base;
}

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