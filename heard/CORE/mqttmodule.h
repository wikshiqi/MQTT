#ifndef MQTTMODULE_H
#define MQTTMODULE_H

#include <QWidget>
#include <QMqttClient>
#include <QMqttMessage>
#include <QMqttTopicName>
#include <QJsonObject>
#include "UI/MqttUI.h"

class MqttModule : public QWidget
{
Q_OBJECT
public:
    explicit MqttModule(MqttUI* ui, QWidget *parent = nullptr);

signals:
    void logInfo(const QString&);
    void logError(const QString&);
    void sendDataToChart(const QJsonObject&);

private slots:
    void connectToMqtt();
    void disconnectFromMqtt();
    void subscribe(const QString&);
    void publish(const QString&, const QByteArray&);
    void onStateChanged(QMqttClient::ClientState);

    // 【关键修复】参数必须完全匹配官方信号
    void onMessageReceived(const QMqttMessage&, const QMqttTopicName&);

    void sendSwitch1();
    void sendSwitch2();

private:
    MqttUI* m_ui;
    QMqttClient* m_client;
    QString m_postTopic;
};

#endif