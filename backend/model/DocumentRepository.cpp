#include "DocumentRepository.h"
#include <iostream>

// ���캯������ָ��·�������ݿ��ļ�
DocumentRepository::DocumentRepository(const std::string& dbPath) : dbPath_(dbPath) {
    // ʹ��sqlite3_open�����ݿ�
    if (sqlite3_open(dbPath.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db_) << std::endl;
        db_ = nullptr;
    }
}

// �����������ر����ݿ�����
DocumentRepository::~DocumentRepository() {
    if (db_) sqlite3_close(db_);
}

// ��ʼ�����ݿ��ṹ���ĵ���Ͱ汾��
bool DocumentRepository::init() {
    const char* docTable = "CREATE TABLE IF NOT EXISTS documents ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT,"
        "content TEXT,"
        "version INTEGER);";
    const char* verTable = "CREATE TABLE IF NOT EXISTS versions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "docId INTEGER,"
        "content TEXT,"
        "author TEXT,"
        "timestamp INTEGER);";
    char* errMsg = nullptr;
    if (sqlite3_exec(db_, docTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Create documents table failed: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    if (sqlite3_exec(db_, verTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Create versions table failed: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// ������ĵ������ݿ�
bool DocumentRepository::addDocument(const Document& doc) {
    const char* sql = "INSERT INTO documents (title, content, version) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, doc.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, doc.content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, doc.version);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// ���������ĵ�����
bool DocumentRepository::updateDocument(const Document& doc) {
    const char* sql = "UPDATE documents SET title=?, content=?, version=? WHERE id=?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, doc.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, doc.content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, doc.version);
    sqlite3_bind_int(stmt, 4, doc.id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// ����ID��ȡ�ĵ���Ϣ
bool DocumentRepository::getDocument(int id, Document& doc) {
    const char* sql = "SELECT id, title, content, version FROM documents WHERE id=?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, id);
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        doc.id = sqlite3_column_int(stmt, 0);
        const char* title_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        doc.title = title_ptr ? title_ptr : "";
        const char* content_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        doc.content = content_ptr ? content_ptr : "";
        doc.version = sqlite3_column_int(stmt, 3);
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

// ��ȡ�����ĵ��б�
std::vector<Document> DocumentRepository::getAllDocuments() {
    std::vector<Document> docs;
    const char* sql = "SELECT id, title, content, version FROM documents;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return docs;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Document doc;
        doc.id = sqlite3_column_int(stmt, 0);
        const char* title_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        doc.title = title_ptr ? title_ptr : "";
        const char* content_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        doc.content = content_ptr ? content_ptr : "";
        doc.version = sqlite3_column_int(stmt, 3);
        docs.push_back(doc);
    }
    sqlite3_finalize(stmt);
    return docs;
}

// ����ĵ��汾��Ϣ
bool DocumentRepository::addVersion(const Version& ver) {
    const char* sql = "INSERT INTO versions (docId, content, author, timestamp) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, ver.docId);
    sqlite3_bind_text(stmt, 2, ver.content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, ver.author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, static_cast<sqlite3_int64>(ver.timestamp));
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// ��ȡָ���ĵ������а汾
std::vector<Version> DocumentRepository::getVersions(int docId) {
    std::vector<Version> vers;
    const char* sql = "SELECT id, docId, content, author, timestamp FROM versions WHERE docId=?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return vers;
    sqlite3_bind_int(stmt, 1, docId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Version ver;
        ver.id = sqlite3_column_int(stmt, 0);
        ver.docId = sqlite3_column_int(stmt, 1);
        const char* content_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        ver.content = content_ptr ? content_ptr : "";
        const char* author_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        ver.author = author_ptr ? author_ptr : "";
        ver.timestamp = static_cast<std::time_t>(sqlite3_column_int64(stmt, 4));
        vers.push_back(ver);
    }
    sqlite3_finalize(stmt);
    return vers;
}

// �ع��ĵ���ָ���汾����
bool DocumentRepository::rollbackDocument(int docId, int versionId) {
    // 1. ��ȡĿ��汾����
    const char* sql = "SELECT content FROM versions WHERE id=? AND docId=?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, versionId);
    sqlite3_bind_int(stmt, 2, docId);
    std::string content;
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* content_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        content = content_ptr ? content_ptr : "";
        found = true;
    }
    sqlite3_finalize(stmt);
    if (!found) return false;
    // 2. �����ĵ�����ΪĿ��汾
    const char* usql = "UPDATE documents SET content=? WHERE id=?;";
    if (sqlite3_prepare_v2(db_, usql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, docId);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

// ͨ���ĵ���������ĵ�ID
int DocumentRepository::getDocIdByTitle(const std::string& title) {
    const char* sql = "SELECT id FROM documents WHERE title=?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    int docId = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        docId = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return docId;
}

// ͨ�������½��ĵ���������ID
int DocumentRepository::createDocumentWithTitle(const std::string& title) {
    const char* sql = "INSERT INTO documents (title, content, version) VALUES (?, '', 1);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    int docId = static_cast<int>(sqlite3_last_insert_rowid(db_));
    sqlite3_finalize(stmt);
    return docId;
}

// ��ȡ�����ĵ���ID�ͱ���
std::vector<std::pair<int, std::string>> DocumentRepository::getAllDocIdTitle() {
    std::vector<std::pair<int, std::string>> docs;
    const char* sql = "SELECT id, title FROM documents;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return docs;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* title_ptr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string title = title_ptr ? title_ptr : "";
        docs.emplace_back(id, title);
    }
    sqlite3_finalize(stmt);
    return docs;
}

// �������ĵ�
bool DocumentRepository::renameDocument(int docId, const std::string& newTitle) {
    const char* sql = "UPDATE documents SET title=? WHERE id=?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, newTitle.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, docId);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
} 