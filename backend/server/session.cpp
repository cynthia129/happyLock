#include "session.h"
#include "../service/CollaborationService.h"
#include "../model/DocumentRepository.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include "server.h"

// 全局文档仓库（实际项目可用单例或依赖注入）
static DocumentRepository g_docRepo("happylock.db");

namespace {
std::string now_time() {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
std::string cyan(const std::string& s) { return "\033[36m" + s + "\033[0m"; }
std::string magenta(const std::string& s) { return "\033[35m" + s + "\033[0m"; }
}

Session::Session(boost::asio::ip::tcp::socket socket, CollaborationService* collabService)
    : socket_(std::move(socket)), collabService_(collabService) {
    static bool inited = false;
    if (!inited) { g_docRepo.init(); inited = true; }
    std::cout << cyan("[Session]") << " [" << now_time() << "] 连接建立: " << socket_.remote_endpoint() << std::endl;
}

Session::~Session() {
    try {
        {
            std::lock_guard<std::mutex> lock(g_conn_mutex);
            --g_conn_count;
        }
        std::cout << magenta("[Session]") << " [" << now_time() << "] 连接关闭: " << socket_.remote_endpoint()
                  << " | 当前连接数: " << g_conn_count << std::endl;
    } catch (...) {
        std::cout << magenta("[Session]") << " [" << now_time() << "] 连接关闭: (未知端点)" << std::endl;
    }
}

void Session::start() {
    do_read();
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buffer_),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                read_buffer_.append(buffer_.data(), length);
                size_t pos;
                while ((pos = read_buffer_.find('\n')) != std::string::npos) {
                    std::string line = read_buffer_.substr(0, pos);
                    read_buffer_.erase(0, pos + 1);
                    try {
                        if (!line.empty()) {
                            auto j = json::parse(line);
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
                                    doc = Document(docId, std::string("文档") + std::to_string(docId), content, 1);
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
                                int newDocId = -1;
                                std::string user = j.value("user", "");
                                if (j.contains("docTitle")) {
                                    std::string docTitle = j.value("docTitle", "");
                                    newDocId = g_docRepo.getDocIdByTitle(docTitle);
                                    if (newDocId == -1) {
                                        newDocId = g_docRepo.createDocumentWithTitle(docTitle);
                                    }
                                } else {
                                    newDocId = j.value("docId", 1);
                                }
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
                            } else if (type == "list_docs") {
                                auto docs = g_docRepo.getAllDocIdTitle();
                                json arr = json::array();
                                for (const auto& d : docs) {
                                    arr.push_back({{"id", d.first}, {"title", d.second}});
                                }
                                json resp = {
                                    {"type", "doc_list"},
                                    {"docs", arr}
                                };
                                do_write(resp.dump());
                            } else if (type == "rename_doc") {
                                int docId = j.value("docId", 0);
                                std::string newTitle = j.value("newTitle", "");
                                bool ok = g_docRepo.renameDocument(docId, newTitle);
                                json resp = {
                                    {"type", "rename_result"},
                                    {"docId", docId},
                                    {"ok", ok},
                                    {"newTitle", newTitle}
                                };
                                do_write(resp.dump());
                            }
                        }
                    } catch (std::exception& e) {
                        std::cerr << "JSON parse error: " << e.what() << std::endl;
                        std::cerr << "原始数据: [" << line << "]" << std::endl;
                    }
                }
                do_read();
            } else {
                std::cerr << "[Session] 连接异常断开: " << ec.message() << std::endl;
            }
        });
}

void Session::do_write(const std::string& msg) {
    auto self(shared_from_this());
    auto data = std::make_shared<std::string>(msg + "\n");
    boost::asio::async_write(socket_, boost::asio::buffer(*data),
        [this, self, data](boost::system::error_code ec, std::size_t /*length*/) {
            // 可选：处理写完成
        });
} 