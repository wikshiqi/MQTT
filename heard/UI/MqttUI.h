#ifndef MQTTUI_H
#define MQTTUI_H

#include <QWidget>
#include <QString>
#include <QByteArray>

struct MqttConfig {
    QString broker;
    int port;
    QString clientId;
    bool autoReconnect;
    QString username;
    QString password;
};

class QLineEdit;
class QPushButton;
class QTextEdit;

class MqttUI : public QWidget
{
Q_OBJECT
public:
    explicit MqttUI(QWidget *parent = nullptr);
    MqttConfig getMqttConfig() const;
    QString getPublishTopic() const;
    QByteArray getPublishMsg() const;
    QString getSubscribeTopic() const;
    void updateMqttState(bool isConnected);
    void appendReceivedMsg(const QString &topic, const QString &msg);
    void showError(const QString &error);

    QLineEdit* m_hostEdit;
    QLineEdit* m_portEdit;
    QLineEdit* m_clientIdEdit;
    QLineEdit* m_usernameEdit;
    QLineEdit* m_passwordEdit;
    QLineEdit* m_topicEdit;
    QLineEdit* m_payloadEdit;
    QPushButton* m_connectBtn;
    QPushButton* m_disconnectBtn;
    QPushButton* m_subscribeBtn;
    QPushButton* m_publishBtn;
    QTextEdit* m_logEdit;

    // ========== 新增：控制按钮 ==========
    QPushButton* m_btnSwitch1;
    QPushButton* m_btnSwitch2;

signals:
    void connectMqtt();
    void disconnectMqtt();
    void publishMessage(const QString& topic, const QByteArray& msg);
    void subscribeTopic(const QString& topic);
    void sendSwitch1();    // 控制开关1
    void sendSwitch2();    // 控制开关2

public slots:
    void connectMqttClicked();
    void disconnectMqttClicked();
    void publishMsgClicked();
    void subscribeTopicClicked();

private:
    void initUI();
    void initStyle();
};

#endif