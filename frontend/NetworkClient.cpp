#include "NetworkClient.h"
#include <QJsonDocument>
#include <QDebug>

// 构造函数，初始化socket并连接信号槽
NetworkClient::NetworkClient(QObject* parent)
    : QObject(parent), socket_(new QTcpSocket(this)) {
    connect(socket_, &QTcpSocket::readyRead, this, &NetworkClient::onReadyRead);
    connect(socket_, &QTcpSocket::connected, this, &NetworkClient::onConnected);
    connect(socket_, &QTcpSocket::disconnected, this, &NetworkClient::onDisconnected);
    connect(socket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkClient::onError);
}

// 连接到指定服务器
void NetworkClient::connectToServer(const QString& host, quint16 port) {
    socket_->connectToHost(host, port);
}

// 发送JSON消息到服务器
bool NetworkClient::sendJson(const QJsonObject& obj) {
    if (!isConnected()) {
        qDebug() << "Cannot send data: socket not connected";
        return false;
    }
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + '\n'; // 用换行分隔消息
    qint64 bytesWritten = socket_->write(data);
    return bytesWritten == data.size();
}

// 判断是否已连接
bool NetworkClient::isConnected() const {
    return socket_->state() == QAbstractSocket::ConnectedState;
}

// 处理服务器发来的数据，按行解析JSON
void NetworkClient::onReadyRead() {
    buffer_.append(socket_->readAll());
    while (true) {
        int idx = buffer_.indexOf('\n');
        if (idx < 0) break;
        QByteArray line = buffer_.left(idx);
        buffer_ = buffer_.mid(idx + 1);
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            emit jsonReceived(doc.object());
        }
    }
}

// 处理连接成功
void NetworkClient::onConnected() { emit connected(); }
// 处理断开连接
void NetworkClient::onDisconnected() { emit disconnected(); }
// 处理网络错误
void NetworkClient::onError(QAbstractSocket::SocketError) {
    emit errorOccurred(socket_->errorString());
} 