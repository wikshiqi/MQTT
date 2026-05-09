#include "BackendClient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocket>

// ====================== 明文地址（不用SSL）======================
#define BACKEND_HTTP_URL  "http://192.168.31.25:8080/iot/upload"
#define BACKEND_WS_URL    "ws://192.168.31.25:8080/iot/ws"
// ==============================================================

BackendClient::BackendClient(QObject *parent)
        : QObject{parent}
{
    m_httpMgr = new QNetworkAccessManager(this);
    m_ws = new QWebSocket;

    connect(m_ws, &QWebSocket::connected, this, &BackendClient::onWsConnected);
    connect(m_ws, &QWebSocket::disconnected, this, &BackendClient::onWsDisconnected);
    connect(m_ws, &QWebSocket::textMessageReceived, this, &BackendClient::onWsTextReceived);

    connectWebSocket();
}

void BackendClient::uploadData(const QString &deviceId, double temp, double humi)
{
    QJsonObject obj;
    obj["deviceId"] = deviceId;
    obj["temp"] = temp;
    obj["humi"] = humi;

    QNetworkRequest req(QUrl(BACKEND_HTTP_URL));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    m_httpMgr->post(req, QJsonDocument(obj).toJson());
}

void BackendClient::connectWebSocket()
{
    emit logInfo("正在连接后端...");
    m_ws->open(QUrl(BACKEND_WS_URL));
}

void BackendClient::onWsConnected()
{
    emit logInfo("✅ 连接后端成功！");
}

void BackendClient::onWsDisconnected()
{
    emit logError("❌ 断开连接");
}

void BackendClient::onWsTextReceived(const QString &msg)
{
    emit logInfo("【推送】" + msg);
    emit wsMessageReceived(msg);
}