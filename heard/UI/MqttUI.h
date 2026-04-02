#ifndef MQTTUI_H
#define MQTTUI_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

struct MqttConfig
{
    QString broker;
    int port;
    QString clientId;
    QString username;
    QString password;
    bool autoReconnect;
};

class MqttUI : public QWidget
{
Q_OBJECT

public:
    explicit MqttUI(QWidget *parent = nullptr);

    MqttConfig getMqttConfig() const;
    QString getPublishTopic() const;
    QByteArray getPublishMsg() const;
    QString getSubscribeTopic() const;

    void updateMqttState(bool connected);
    void appendReceivedMsg(const QString &topic, const QString &msg);
    void showError(const QString &err);
    void showInfo(const QString &info);  // 新增：显示普通信息

signals:
    void connectMqtt();
    void disconnectMqtt();
    void subscribeTopic(const QString &topic);
    void publishMessage(const QString &topic, const QByteArray &payload);
    void sendSwitch1();
    void sendSwitch2();

private slots:
    void connectMqttClicked();
    void disconnectMqttClicked();
    void publishMsgClicked();
    void subscribeTopicClicked();
    void sendSwitch1d();
    void sendSwitch2d();

private:
    void initUI();
    void initStyle();

    QLineEdit *m_hostEdit;
    QLineEdit *m_portEdit;
    QLineEdit *m_clientIdEdit;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_topicEdit;
    QLineEdit *m_payloadEdit;
    QPushButton *m_connectBtn;
    QPushButton *m_disconnectBtn;
    QPushButton *m_subscribeBtn;
    QPushButton *m_publishBtn;
    QPushButton *m_btnSwitch1;
    QPushButton *m_btnSwitch2;
    QTextEdit *m_logEdit;
};

#endif // MQTTUI_H