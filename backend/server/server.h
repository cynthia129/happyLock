#pragma once
#include <boost/asio.hpp>
#include <memory>
#include "session.h"
#include "../service/CollaborationService.h"
#include <mutex>

// Server�ฺ������TCP�����������ܿͻ������ӣ���Ϊÿ�����Ӵ���Session����
class Server {
public:
    // ���캯������ʼ�������˿�
    Server(boost::asio::io_context& io_context, short port);

private:
    // �첽����������
    void do_accept();
    boost::asio::ip::tcp::acceptor acceptor_; // TCP���ӽ�����
    CollaborationService collabService_;      // Эͬ������󣬹�������Session
};

// ȫ���������������ͼ�����
extern std::mutex g_conn_mutex; // ����g_conn_count�Ļ�����
extern int g_conn_count;        // ��ǰ��Ծ������ 