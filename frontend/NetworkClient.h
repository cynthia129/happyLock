#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

// NetworkClient类用于管理与后端服务器的TCP连接，发送和接收JSON消息。
class NetworkClient : public QObject {
    Q_OBJECT
public:
    // 构造函数，初始化socket
    explicit NetworkClient(QObject* parent = nullptr);

    // 连接到服务器
    void connectToServer(const QString& host, quint16 port);
    // 发送JSON消息
    bool sendJson(const QJsonObject& obj);
    // 判断是否已连接
    bool isConnected() const;

signals:
    void connected();                // 连接成功信号
    void disconnected();             // 断开连接信号
    void jsonReceived(const QJsonObject& obj); // 收到JSON消息信号
    void errorOccurred(const QString& err);    // 网络错误信号

private slots:
    void onReadyRead();              // 处理数据到达
    void onConnected();              // 处理连接成功
    void onDisconnected();           // 处理断开连接
    void onError(QAbstractSocket::SocketError); // 处理错误

private:
    QTcpSocket* socket_;             // TCP套接字
    QByteArray buffer_;              // 接收缓冲区
}; 