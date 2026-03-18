#ifndef MQTTMODULE_H
#define MQTTMODULE_H

#include <QWidget>
#include <QTcpSocket>
#include <QTimer>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <UI/MqttUI.h>
// MQTT报文类型
enum MqttPacketType {
    CONNECT = 1, CONNACK = 2, PUBLISH = 3, SUBSCRIBE = 8, SUBACK = 9, PINGREQ = 12, PINGRESP = 13
};

class MqttModule : public QWidget
{
Q_OBJECT
public:
    explicit MqttModule(MqttUI* ui, QWidget *parent = nullptr);
    ~MqttModule() override;

signals:
    void logInfo(const QString& info);       // 日志信号
    void logError(const QString& error);     // 错误信号

private slots:
    // 界面操作
    void onConnectClicked();
    void onDisconnectClicked();
    void onSubscribeClicked();
    void onPublishClicked();
    // MQTT核心逻辑
    void onSocketStateChanged(QAbstractSocket::SocketState state);
    void onReadyRead();
    void onPingTimerTimeout();

private:
    // 界面控件
//    QLineEdit* m_hostEdit;
//    QLineEdit* m_portEdit;
//    QLineEdit* m_clientIdEdit;
//    QLineEdit* m_usernameEdit;
//    QLineEdit* m_passwordEdit;
//    QLineEdit* m_topicEdit;
//    QLineEdit* m_payloadEdit;
//    QTextEdit* m_logEdit;
//    QPushButton* m_connectBtn;
//    QPushButton* m_disconnectBtn;
//    QPushButton* m_subscribeBtn;
//    QPushButton* m_publishBtn;
    MqttUI* mqttUi;
    // MQTT核心变量
    QTcpSocket* m_socket;
    QTimer* m_pingTimer;
    quint16 m_nextPacketId;
    quint32 m_keepAlive;

    // MQTT协议封装
    QByteArray encodeFixedHeader(MqttPacketType type, quint32 remainingLength);
    QByteArray encodeRemainingLength(quint32 length);
    QByteArray buildConnectPacket(const QString& clientId, const QString& username, const QString& password);
    QByteArray buildSubscribePacket(const QString& topic, quint8 qos, quint16 packetId);
    QByteArray buildPublishPacket(const QString& topic, const QByteArray& payload, quint8 qos, bool retain, quint16 packetId);
    // MQTT协议解析
    void parseConnAck(const QByteArray& data);
    void parseSubAck(const QByteArray& data);
    void parsePublish(const QByteArray& data);
};

#endif // MQTTMODULE_H