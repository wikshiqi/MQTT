#include "BackendClient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#define BACKEND_HTTP_URL  "http://192.168.31.27:8080/iot/upload"
#define BACKEND_WS_URL    "ws://192.168.31.27:8080/iot/ws"

BackendClient::BackendClient(QObject *parent)
        : QObject{parent}
{
    m_httpMgr = new QNetworkAccessManager(this);
    m_ws = new QWebSocket();
    m_reconnectTimer = new QTimer(this);

    // 连接成功
    connect(m_ws, &QWebSocket::connected, this, [this]() {
        emit logInfo("✅ WebSocket 连接 SpringBoot 成功！");
        m_reconnectTimer->stop();
    });

    // 断开连接
    connect(m_ws, &QWebSocket::disconnected, this, [this]() {
        emit logError("❌ WebSocket 断开，3秒后重连...");
        m_reconnectTimer->start(3000);
    });

    // 收到消息
    connect(m_ws, &QWebSocket::textMessageReceived, this, [this](const QString& msg) {
        emit logInfo("📩 SpringBoot 推送：" + msg);
        emit wsMessageReceived(msg);
    });

    // 自动重连
    connect(m_reconnectTimer, &QTimer::timeout, this, [this]() {
        m_ws->open(QUrl(BACKEND_WS_URL));
    });

    // 开始连接
    m_ws->open(QUrl(BACKEND_WS_URL));
    emit logInfo("正在连接 SpringBoot...");
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

    emit logInfo("📤 温湿度已上传至 SpringBoot");
}