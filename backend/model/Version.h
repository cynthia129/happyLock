#pragma once
#include <string>
#include <ctime>

// Version���ʾ�ĵ���һ����ʷ�汾�������汾ID�������ĵ�ID�����ݡ����ߺ�ʱ�����
class Version {
public:
    int id;                // �汾Ψһ��ʶ��
    int docId;             // �����ĵ���ID
    std::string content;   // �ð汾���ĵ�����
    std::string author;    // �༭�ð汾������
    std::time_t timestamp; // �汾����ʱ���

    // Ĭ�Ϲ��캯���������հ汾����
    Version() = default;
    // ���������캯������ʼ�����г�Ա����
    Version(int id, int docId, const std::string& content, const std::string& author, std::time_t timestamp);
}; 