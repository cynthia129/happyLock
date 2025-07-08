#pragma once
#include "Document.h"
#include "Version.h"
#include <vector>
#include <string>
#include <sqlite3.h>

// DocumentRepository类用于管理文档和版本的持久化，封装了所有与SQLite数据库的交互。
class DocumentRepository {
public:
    // 构造函数，传入数据库文件路径
    DocumentRepository(const std::string& dbPath);
    // 析构函数，关闭数据库连接
    ~DocumentRepository();

    // 初始化数据库表结构
    bool init();
    // 添加新文档
    bool addDocument(const Document& doc);
    // 更新已有文档
    bool updateDocument(const Document& doc);
    // 根据ID获取文档
    bool getDocument(int id, Document& doc);
    // 获取所有文档
    std::vector<Document> getAllDocuments();

    // 添加文档版本
    bool addVersion(const Version& ver);
    // 获取指定文档的所有版本
    std::vector<Version> getVersions(int docId);

    // 回滚文档到指定版本
    bool rollbackDocument(int docId, int versionId);

    // 通过文档名查找文档ID
    int getDocIdByTitle(const std::string& title);
    // 通过文档名新建文档并返回ID
    int createDocumentWithTitle(const std::string& title);

    // 获取所有文档的ID和标题
    std::vector<std::pair<int, std::string>> getAllDocIdTitle();
    // 重命名文档
    bool renameDocument(int docId, const std::string& newTitle);

private:
    sqlite3* db_ = nullptr;      // SQLite数据库连接指针
    std::string dbPath_;        // 数据库文件路径
}; 