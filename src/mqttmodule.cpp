#include "mqttmodule.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMqttTopicFilter>
#include <QMqttMessage>

MqttModule::MqttModule(MqttUI* ui, QWidget *parent)
        : QWidget(parent), m_ui(ui)
{
    m_client = new QMqttClient(this);
    m_postTopic = "$sys/5Tgf5AGpeZ/DHT11/thing/property/post";

    // ===================== 修复：全部使用 Qt5 兼容的信号槽写法 =====================
    connect(m_ui, SIGNAL(connectMqtt()), this, SLOT(connectToMqtt()));
    connect(m_ui, SIGNAL(disconnectMqtt()), this, SLOT(disconnectFromMqtt()));
    connect(m_ui, SIGNAL(subscribeTopic(QString)), this, SLOT(subscribe(QString)));
    connect(m_ui, SIGNAL(publishMessage(QString,QByteArray)), this, SLOT(publish(QString,QByteArray)));
    connect(m_ui, SIGNAL(sendSwitch1()), this, SLOT(sendSwitch1()));
    connect(m_ui, SIGNAL(sendSwitch2()), this, SLOT(sendSwitch2()));

    connect(m_client, SIGNAL(stateChanged(QMqttClient::ClientState)), this, SLOT(onStateChanged(QMqttClient::ClientState)));
    connect(m_client, SIGNAL(messageReceived(QMqttMessage,QMqttTopicName)), this, SLOT(onMessageReceived(QMqttMessage,QMqttTopicName)));
}

void MqttModule::connectToMqtt() {
    MqttConfig c = m_ui->getMqttConfig();
    m_client->setHostname(c.broker);
    m_client->setPort(c.port);
    m_client->setClientId(c.clientId);
    m_client->setUsername(c.username);
    m_client->setPassword(c.password);
    m_client->connectToHost();
    emit logInfo("正在连接 OneNET...");
}

void MqttModule::disconnectFromMqtt() {
    m_client->disconnectFromHost();
}

void MqttModule::subscribe(const QString& t) {
    m_client->subscribe(QMqttTopicFilter(t), 0);
}

void MqttModule::publish(const QString& t, const QByteArray& d) {
    m_client->publish(t, d);
}

void MqttModule::onStateChanged(QMqttClient::ClientState s) {
    bool ok = (s == QMqttClient::Connected);
    m_ui->updateMqttState(ok);
    if(ok)
            emit logInfo("MQTT 已连接");
    else
            emit logInfo("MQTT 已断开");
}

// ===================== 修复：参数必须完全匹配 QMqtt 官方信号 =====================
void MqttModule::onMessageReceived(const QMqttMessage &msg, const QMqttTopicName &topicName)
{
    QString topic = topicName.name();
    QString payload = QString::fromUtf8(msg.payload());

    m_ui->appendReceivedMsg(topic, payload);

    // 解析JSON并发送给图表
    QJsonParseError err{};
    QJsonDocument doc = QJsonDocument::fromJson(msg.payload(), &err);
    if (!err.error && doc.isObject()) {
        emit sendDataToChart(doc.object());
    }
}

void MqttModule::sendSwitch1() {
    m_client->publish(m_postTopic, "{\"switch1\":1}");
}
void MqttModule::sendSwitch2() {
    m_client->publish(m_postTopic, "{\"switch2\":1}");
}