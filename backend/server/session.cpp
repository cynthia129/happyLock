#include "session.h"
#include "../service/CollaborationService.h"
#include "../model/DocumentRepository.h"
#include <iostream>

// 全局文档仓库（实际项目可用单例或依赖注入）
static DocumentRepository g_docRepo("happylock.db");

Session::Session(boost::asio::ip::tcp::socket socket, CollaborationService* collabService)
    : socket_(std::move(socket)), collabService_(collabService) {
    static bool inited = false;
    if (!inited) { g_docRepo.init(); inited = true; }
}

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
                        int recvDocId = j.value("docId", 1);
                        std::string content = j.value("content", "");
                        std::string user = j.value("user", "");
                        docId = recvDocId;
                        username = user;
                        // 保存内容到数据库
                        Document doc;
                        if (!g_docRepo.getDocument(docId, doc)) {
                            doc = Document(docId, "文档" + std::to_string(docId), content, 1);
                            g_docRepo.addDocument(doc);
                        } else {
                            doc.content = content;
                            doc.version++;
                            g_docRepo.updateDocument(doc);
                        }
                        // 保存版本快照
                        Version ver(0, docId, content, username, std::time(nullptr));
                        g_docRepo.addVersion(ver);
                        // 广播给同文档其他Session
                        json resp = {
                            {"type", "edit"},
                            {"docId", docId},
                            {"user", username},
                            {"content", content}
                        };
                        collabService_->broadcast(docId, resp.dump(), self);
                        collabService_->broadcastUserList(docId);
                    } else if (type == "switch_doc") {
                        int newDocId = j.value("docId", 1);
                        std::string user = j.value("user", "");
                        // 退出旧文档
                        collabService_->removeSession(docId, self);
                        collabService_->broadcastUserList(docId);
                        docId = newDocId;
                        username = user;
                        collabService_->addSession(docId, self);
                        collabService_->broadcastUserList(docId);
                        // 返回当前文档内容
                        Document doc;
                        std::string content;
                        if (g_docRepo.getDocument(docId, doc)) {
                            content = doc.content;
                        } else {
                            content = "";
                        }
                        json resp = {
                            {"type", "switch_doc"},
                            {"docId", docId},
                            {"user", username},
                            {"content", content}
                        };
                        do_write(resp.dump());
                    } else if (type == "get_versions") {
                        int reqDocId = j.value("docId", 1);
                        auto vers = g_docRepo.getVersions(reqDocId);
                        json arr = json::array();
                        for (const auto& v : vers) {
                            arr.push_back({
                                {"id", v.id},
                                {"author", v.author},
                                {"timestamp", v.timestamp}
                            });
                        }
                        json resp = {
                            {"type", "versions"},
                            {"docId", reqDocId},
                            {"versions", arr}
                        };
                        do_write(resp.dump());
                    } else if (type == "rollback") {
                        int reqDocId = j.value("docId", 1);
                        int versionId = j.value("versionId", 0);
                        bool ok = g_docRepo.rollbackDocument(reqDocId, versionId);
                        json resp = {
                            {"type", "rollback_result"},
                            {"docId", reqDocId},
                            {"versionId", versionId},
                            {"ok", ok}
                        };
                        do_write(resp.dump());
                        // 回滚成功后广播新内容
                        if (ok) {
                            Document doc;
                            if (g_docRepo.getDocument(reqDocId, doc)) {
                                json editMsg = {
                                    {"type", "edit"},
                                    {"docId", reqDocId},
                                    {"user", username},
                                    {"content", doc.content}
                                };
                                collabService_->broadcast(reqDocId, editMsg.dump(), nullptr);
                            }
                        }
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
    boost::asio::async_write(socket_, boost::asio::buffer(msg + "\n"),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            // 可选：处理写完成
        });
} 