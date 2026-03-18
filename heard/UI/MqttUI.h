#ifndef MQTTUI_H
#define MQTTUI_H

#include <QWidget>
#include <QString>
#include <QByteArray>

// 定义MQTT配置结构体
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
Q_OBJECT // 必须添加Q_OBJECT宏以支持信号槽

public:
    explicit MqttUI(QWidget *parent = nullptr);
    MqttConfig getMqttConfig() const;
    QString getPublishTopic() const;
    QByteArray getPublishMsg() const;
    QString getSubscribeTopic() const;
    void updateMqttState(bool isConnected);
    void appendReceivedMsg(const QString &topic, const QString &msg);
    void showError(const QString &error);

    // 界面控件（与cpp中定义的变量名保持一致）
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
signals:
    // 声明缺失的信号
    void connectMqtt();
    void disconnectMqtt();
    void publishMessage(const QString& topic, const QByteArray& msg);
    void subscribeTopic(const QString& topic);

public slots:
    void connectMqttClicked(void);
    void disconnectMqttClicked(void);
    void publishMsgClicked(void);
    void subscribeTopicClicked(void);

private:
    void initUI();
    void initStyle();


};

#endif // MQTTUI_H