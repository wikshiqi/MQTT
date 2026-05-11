#include "BackendClient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWebSocket>
#include <QDebug>

// ====================== 明文 WS 地址（不用SSL！）======================
#define BACKEND_HTTP_URL  "http://192.168.31.25:8080/iot/upload"
#define BACKEND_WS_URL    "ws://192.168.31.25:8080/iot/ws"
// ====================================================================

BackendClient::BackendClient(QObject *parent)
        : QObject{parent}
{
    m_httpMgr = new QNetworkAccessManager(this);
    m_ws = new QWebSocket;

    // 连接信号
    connect(m_ws, &QWebSocket::connected, this, [this]() {
        emit logInfo("✅ QT <-> SpringBoot WebSocket 连接成功！");
    });
    connect(m_ws, &QWebSocket::disconnected, this, [this]() {
        emit logError("❌ WebSocket 断开");
    });
    connect(m_ws, &QWebSocket::textMessageReceived, this, [this](const QString& msg) {
        emit logInfo("📩 SpringBoot 推送：" + msg);
        emit wsMessageReceived(msg);
    });

    // 连接 SpringBoot
    m_ws->open(QUrl(BACKEND_WS_URL));
    emit logInfo("正在连接 SpringBoot 后端...");
}

// 上传温湿度数据到 SpringBoot
void BackendClient::uploadData(const QString &deviceId, double temp, double humi)
{
    QJsonObject obj;
    obj["deviceId"] = deviceId;
    obj["temp"] = temp;
    obj["humi"] = humi;

    QNetworkRequest req(QUrl(BACKEND_HTTP_URL));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    m_httpMgr->post(req, QJsonDocument(obj).toJson());
    emit logInfo("📤 已上传温湿度到 SpringBoot");
}

// 发送指令给 SpringBoot
void BackendClient::sendCmd(const QString& cmd)
{
    if (m_ws->state() == QAbstractSocket::ConnectedState) {
        m_ws->sendTextMessage(cmd);
    }
}