#include "NetworkClient.h"
#include <QJsonDocument>
#include <QDebug>

NetworkClient::NetworkClient(QObject* parent)
    : QObject(parent), socket_(new QTcpSocket(this)) {
    connect(socket_, &QTcpSocket::readyRead, this, &NetworkClient::onReadyRead);
    connect(socket_, &QTcpSocket::connected, this, &NetworkClient::onConnected);
    connect(socket_, &QTcpSocket::disconnected, this, &NetworkClient::onDisconnected);
    connect(socket_, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkClient::onError);
}

void NetworkClient::connectToServer(const QString& host, quint16 port) {
    socket_->connectToHost(host, port);
}

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

bool NetworkClient::isConnected() const {
    return socket_->state() == QAbstractSocket::ConnectedState;
}

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

void NetworkClient::onConnected() { emit connected(); }
void NetworkClient::onDisconnected() { emit disconnected(); }
void NetworkClient::onError(QAbstractSocket::SocketError) {
    emit errorOccurred(socket_->errorString());
} 