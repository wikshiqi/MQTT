#ifndef MQTTMODULE_H
#define MQTTMODULE_H

#include <QWidget>
#include <QMqttClient>
#include <QMqttMessage>
#include <UI/MqttUI.h>
#include <QJsonObject>
#include <QWebSocket>  // 必须加！
#include "BackendClient.h"
class BackendClient;

class MqttModule : public QWidget
{
Q_OBJECT
public:
    explicit MqttModule(MqttUI* ui, QWidget *parent = nullptr);
    ~MqttModule() override;
void onClientError(QMqttClient::ClientError error);
signals:
    void logInfo(const QString& info);
    void logError(const QString& error);
    void sendDataToChart(const QJsonObject& data);

private slots:
    void onConnectClicked();
    void onDisconnectClicked();
    void onSubscribeClicked();
    void onPublishClicked();
    void onClientConnected();
    void onClientDisconnected();
    void onMessageReceived(const QByteArray &payload, const QMqttTopicName &topic);


private:
    MqttUI* mqttUi;
    QMqttClient* m_client;
    QString post_topic = "$sys/5Tgf5AGpeZ/DHT11/thing/event/property/post";
    BackendClient *BackWebsocket;
    QTextEdit* m_globalLogEdit;
};

#endif // MQTTMODULE_H