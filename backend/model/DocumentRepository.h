#pragma once
#include "Document.h"
#include "Version.h"
#include <vector>
#include <string>
#include <sqlite3.h>

// DocumentRepository�����ڹ����ĵ��Ͱ汾�ĳ־û�����װ��������SQLite���ݿ�Ľ�����
class DocumentRepository {
public:
    // ���캯�����������ݿ��ļ�·��
    DocumentRepository(const std::string& dbPath);
    // �����������ر����ݿ�����
    ~DocumentRepository();

    // ��ʼ�����ݿ��ṹ
    bool init();
    // ������ĵ�
    bool addDocument(const Document& doc);
    // ���������ĵ�
    bool updateDocument(const Document& doc);
    // ����ID��ȡ�ĵ�
    bool getDocument(int id, Document& doc);
    // ��ȡ�����ĵ�
    std::vector<Document> getAllDocuments();

    // ����ĵ��汾
    bool addVersion(const Version& ver);
    // ��ȡָ���ĵ������а汾
    std::vector<Version> getVersions(int docId);

    // �ع��ĵ���ָ���汾
    bool rollbackDocument(int docId, int versionId);

    // ͨ���ĵ��������ĵ�ID
    int getDocIdByTitle(const std::string& title);
    // ͨ���ĵ����½��ĵ�������ID
    int createDocumentWithTitle(const std::string& title);

    // ��ȡ�����ĵ���ID�ͱ���
    std::vector<std::pair<int, std::string>> getAllDocIdTitle();
    // �������ĵ�
    bool renameDocument(int docId, const std::string& newTitle);

private:
    sqlite3* db_ = nullptr;      // SQLite���ݿ�����ָ��
    std::string dbPath_;        // ���ݿ��ļ�·��
}; 