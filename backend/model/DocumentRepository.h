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

private:
    sqlite3* db_ = nullptr;
    std::string dbPath_;
}; 