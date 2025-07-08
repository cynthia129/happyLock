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
    //先判断是否连接
    if (!isConnected()) {
        qDebug() << "Cannot send data: socket not connected";
        return false;
    }
    QJsonDocument doc(obj);
    //将jsonobj转换为jsonDoc便于传输，传输格式为紧凑型，不带多余空格换行和缩进
    QByteArray data = doc.toJson(QJsonDocument::Compact) + '\n'; // 用换行分隔消息
    //将数据写入socket，返回值为实际写入数据的字节数
    qint64 bytesWritten = socket_->write(data);
    //返回实际写入字节数和期望写入字节数是否一致
    return bytesWritten == data.size();
}

// 判断是否已连接
bool NetworkClient::isConnected() const {
    return socket_->state() == QAbstractSocket::ConnectedState;
}

// 处理服务器发来的数据，按行解析JSON
void NetworkClient::onReadyRead() {
    //将socket数据全部读出放入缓冲区
    buffer_.append(socket_->readAll());
    while (true) {
        //读取有多少行数据
        int idx = buffer_.indexOf('\n');
        //如果数据小于一行，return，待数据更多时再读
        if (idx < 0) break;
        //读取一行数据（left的意思是从开头读到idx的位置）
        QByteArray line = buffer_.left(idx);
        //从buffer中删掉前一行，意思是将idx+1到末尾截取出来
        buffer_ = buffer_.mid(idx + 1);
        QJsonParseError err;
        //将当前行从json转换为jsonDoc
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        //判断正确性，正确就发送json收到了的信号，将当前由buffer处理好了的jsonDoc发给槽函数处理
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