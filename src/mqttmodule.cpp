#include "mqttmodule.h"
#include "MqttUI.h"
#include <QJsonDocument>
#include <QMqttTopicFilter>
#include <QMqttSubscription>
#include <QJsonObject>
#include <QDebug>

MqttModule::MqttModule(MqttUI* ui, QWidget *parent)
        : QWidget(parent), m_ui(ui), m_client(nullptr)
{
    m_client = new QMqttClient(this);
    if (!m_client) {
        m_ui->showError("FATAL: 无法创建 QMqttClient！");
        return;
    }

    m_postTopic = "$/sys/5Tgf5AGpeZ/DHT11/thing/event/property/post";

    if (!m_ui) {
        qWarning() << "FATAL: MqttUI pointer is null!";
        return;
    }

    // 连接 UI 信号
    connect(m_ui, &MqttUI::connectMqtt,       this, &MqttModule::connectToMqtt);
    connect(m_ui, &MqttUI::disconnectMqtt,    this, &MqttModule::disconnectFromMqtt);
    connect(m_ui, &MqttUI::subscribeTopic,    this, &MqttModule::subscribe);
    connect(m_ui, &MqttUI::publishMessage,    this, &MqttModule::publish);
    connect(m_ui, &MqttUI::sendSwitch1,       this, &MqttModule::sendSwitch1);
    connect(m_ui, &MqttUI::sendSwitch2,       this, &MqttModule::sendSwitch2);

    // 连接 m_client 信号
    connect(m_client, &QMqttClient::stateChanged,    this, &MqttModule::onStateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &MqttModule::onMessageReceived);
}

MqttModule::~MqttModule()
{
    // m_client 作为子对象自动析构
}

void MqttModule::connectToMqtt()
{
    if (!m_client) {
        m_ui->showError("MQTT client 不可用");
        return;
    }
    auto c = m_ui->getMqttConfig();
    m_client->setHostname(c.broker);
    m_client->setPort(c.port);
    m_client->setClientId(c.clientId);
    m_client->setUsername(c.username);
    m_client->setPassword(c.password);
    m_client->connectToHost();
    m_ui->showInfo("正在连接 OneNET...");
}

void MqttModule::disconnectFromMqtt()
{
    if (m_client) {
        m_client->disconnectFromHost();
    }
}

void MqttModule::subscribe(const QString& t)
{
    if (!m_client) {
        m_ui->showError("MQTT client 不可用");
        return;
    }
    if (m_client->state() != QMqttClient::Connected) {
        m_ui->showError("MQTT 未连接，无法订阅");
        return;
    }
    QMqttTopicFilter filter(t);
    QMqttSubscription *sub = m_client->subscribe(filter);
    if (!sub) {
        m_ui->showError(QString("订阅失败：无效主题或网络错误 (%1)").arg(t));
    } else {
        m_ui->showInfo(QString("已发送订阅请求，主题：%1").arg(t));
    }
}

void MqttModule::publish(const QString& t, const QByteArray& d)
{
    if (!m_client) {
        m_ui->showError("MQTT client 不可用");
        return;
    }
    if (m_client->state() != QMqttClient::Connected) {
        m_ui->showError("MQTT 未连接，无法发布");
        return;
    }
    qint32 pubId = m_client->publish(t, d);
    if (pubId == -1) {
        m_ui->showError(QString("发布失败：主题 %1").arg(t));
    } else {
        m_ui->showInfo(QString("已发布消息，发布ID：%1").arg(pubId));
    }
}

void MqttModule::sendSwitch1()
{
    if (m_client && m_client->state() == QMqttClient::Connected) {
        m_client->publish(m_postTopic, "{\"switch1\":1}");
        m_ui->showInfo("已发送开关1指令");
    } else {
        m_ui->showError("MQTT 未连接，无法发送开关指令");
    }
}

void MqttModule::sendSwitch2()
{
    if (m_client && m_client->state() == QMqttClient::Connected) {
        m_client->publish(m_postTopic, "{\"switch2\":1}");
        m_ui->showInfo("已发送开关2指令");
    } else {
        m_ui->showError("MQTT 未连接，无法发送开关指令");
    }
}

void MqttModule::onStateChanged(QMqttClient::ClientState s)
{
    bool connected = (s == QMqttClient::Connected);
    m_ui->updateMqttState(connected);
    if (connected) {
        m_ui->showInfo("MQTT 已连接");
    } else {
        m_ui->showInfo("MQTT 已断开");
    }
}

void MqttModule::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    m_ui->appendReceivedMsg(topic.name(), QString::fromUtf8(message));

    QJsonParseError err{};
    auto doc = QJsonDocument::fromJson(message, &err);
    if (!err.error && doc.isObject()) {
        emit sendDataToChart(doc.object());
    } else if (err.error) {
        m_ui->showInfo(QString("收到非JSON消息: %1").arg(QString::fromUtf8(message)));
    }
}