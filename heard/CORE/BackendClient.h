#ifndef BACKENDCLIENT_H
#define BACKENDCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QWebSocket>
#include <QTimer>

class BackendClient : public QObject
{
Q_OBJECT
public:
    explicit BackendClient(QObject *parent = nullptr);
    void uploadData(const QString &deviceId, double temp, double humi);

signals:
    void logInfo(const QString &info);
    void logError(const QString &error);
    void wsMessageReceived(const QString &msg);

private:
    QNetworkAccessManager *m_httpMgr;
    QWebSocket *m_ws;
    QTimer *m_reconnectTimer; // 这里加上！
};

#endif // BACKENDCLIENT_H