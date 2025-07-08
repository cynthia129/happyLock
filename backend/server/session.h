#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <array>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class CollaborationService; // ǰ������

// Session���ʾһ���ͻ��˻Ự�������շ���Ϣ����Эͬ���񽻻���
class Session : public std::enable_shared_from_this<Session> {
public:
    // ���캯������ʼ��socket��Эͬ����ָ��
    Session(boost::asio::ip::tcp::socket socket, CollaborationService* collabService);
    // �����������������ӹر�
    ~Session();
    // �����Ự����ʼ�첽��ȡ
    void start();
    // ������Ϣ���ͻ���
    void do_write(const std::string& msg); // �����ӿ�

    int docId = 1;            // ��ǰ�Ự�����ĵ�ID
    std::string username;     // ��ǰ�Ự�û���

private:
    // �첽��ȡ�ͻ�����Ϣ
    void do_read();

    boost::asio::ip::tcp::socket socket_;      // TCP����socket
    std::array<char, 1024> buffer_;            // ��������
    CollaborationService* collabService_;      // Эͬ����ָ��
    std::string read_buffer_;                  // ��Ϣƴ�ӻ���
}; 