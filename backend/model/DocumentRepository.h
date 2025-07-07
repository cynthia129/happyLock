#pragma once
#include "Document.h"
#include "Version.h"
#include <vector>
#include <string>
#include <sqlite3.h>

class DocumentRepository {
public:
    DocumentRepository(const std::string& dbPath);
    ~DocumentRepository();

    bool init();
    bool addDocument(const Document& doc);
    bool updateDocument(const Document& doc);
    bool getDocument(int id, Document& doc);
    std::vector<Document> getAllDocuments();

    bool addVersion(const Version& ver);
    std::vector<Version> getVersions(int docId);

    bool rollbackDocument(int docId, int versionId);

    // 新增：通过文档名查找文档ID
    int getDocIdByTitle(const std::string& title);
    // 新增：通过文档名新建文档并返回ID
    int createDocumentWithTitle(const std::string& title);

    // 新增：获取所有文档（id和title）
    std::vector<std::pair<int, std::string>> getAllDocIdTitle();
    // 新增：重命名文档
    bool renameDocument(int docId, const std::string& newTitle);

private:
    sqlite3* db_ = nullptr;
    std::string dbPath_;
}; 