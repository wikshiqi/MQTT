#ifndef BACKENDCLIENT_H
#define BACKENDCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QWebSocket>



class BackendClient : public QObject
{
Q_OBJECT
public:
    explicit BackendClient(QObject *parent = nullptr);

    // 上传数据到 SpringBoot
    void uploadData(const QString &deviceId, double temp, double humi);

    // 连接 WebSocket
    void connectWebSocket();

signals:
    void logInfo(const QString &info);
    void logError(const QString &error);
    void wsMessageReceived(const QString &msg);

private slots:
    void onWsConnected();
    void onWsDisconnected();
    void onWsTextReceived(const QString &msg);

private:
    QNetworkAccessManager *m_httpMgr;
    QWebSocket *m_ws;
};

#endif // BACKENDCLIENT_H