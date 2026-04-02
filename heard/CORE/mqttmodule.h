#ifndef MQTTMODULE_H
#define MQTTMODULE_H

#include <QWidget>
#include <QMqttClient>
class MqttUI;

class MqttModule : public QWidget
{
Q_OBJECT
public:
    explicit MqttModule(MqttUI* ui, QWidget *parent = nullptr);
    ~MqttModule();

signals:
    void logInfo(const QString& msg);
    void logError(const QString& msg);
    void sendDataToChart(const QJsonObject& data);

private slots:
    void connectToMqtt();
    void disconnectFromMqtt();
    void subscribe(const QString& topic);
    void publish(const QString& topic, const QByteArray& data);
    void sendSwitch1();
    void sendSwitch2();
    void onStateChanged(QMqttClient::ClientState state);
    void onMessageReceived(const QByteArray& message, const QMqttTopicName& topic);

private:
    MqttUI* m_ui;
    QMqttClient* m_client;
    QString m_postTopic;

};

#endif // MQTTMODULE_H