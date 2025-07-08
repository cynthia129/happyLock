#pragma once
#include <string>

// Document���ʾһ���ĵ����󣬰����ĵ���ID�����⡢���ݺͰ汾�š�
class Document {
public:
    int id;                // �ĵ�Ψһ��ʶ��
    std::string title;     // �ĵ�����
    std::string content;   // �ĵ�����
    int version;           // ��ǰ�ĵ��汾��

    // Ĭ�Ϲ��캯�����������ĵ�����
    Document() = default;
    // ���������캯������ʼ�����г�Ա����
    Document(int id, const std::string& title, const std::string& content, int version);
}; 