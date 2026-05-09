#include "BackendClient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocket>
#include <QSslConfiguration>   // 必须加
#include <QSslSocket>

// ====================== 你的后端地址 ======================
#define BACKEND_HTTP_URL  "http://192.168.31.25:8080/iot/upload"
#define BACKEND_WS_URL    "ws://192.168.31.25:8080/iot/ws"
// ==========================================================

BackendClient::BackendClient(QObject *parent)
        : QObject{parent}
{
    m_httpMgr = new QNetworkAccessManager(this);
    m_ws = new QWebSocket;

    // ✅ 关键：关闭 SSL 错误干扰（解决你所有报错）
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    m_ws->setSslConfiguration(config);

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
    emit logInfo("已上传到后端：" + deviceId + " 温度=" + QString::number(temp));
}

void BackendClient::connectWebSocket()
{
    emit logInfo("正在连接后端 WebSocket...");
    m_ws->open(QUrl(BACKEND_WS_URL));
}

void BackendClient::onWsConnected()
{
    emit logInfo("✅ WebSocket 已连接后端");
}

void BackendClient::onWsDisconnected()
{
    emit logError("❌ WebSocket 断开连接");
}

void BackendClient::onWsTextReceived(const QString &msg)
{
    emit logInfo("【后端推送】" + msg);
    emit wsMessageReceived(msg);
}