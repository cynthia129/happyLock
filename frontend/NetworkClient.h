#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class NetworkClient : public QObject {
    Q_OBJECT
public:
    explicit NetworkClient(QObject* parent = nullptr);

    void connectToServer(const QString& host, quint16 port);
    bool sendJson(const QJsonObject& obj);
    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void jsonReceived(const QJsonObject& obj);
    void errorOccurred(const QString& err);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError);

private:
    QTcpSocket* socket_;
    QByteArray buffer_;
}; 