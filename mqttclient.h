#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QTimer>

enum MqttPacketType {
    CONNECT = 1,
    CONNACK = 2,
    PUBLISH = 3,
    PUBACK = 4,
    SUBSCRIBE = 8,
    SUBACK = 9,
    PINGREQ = 12,
    PINGRESP = 13,
    DISCONNECT = 14
};

class MqttClient : public QObject
{
Q_OBJECT
public:
    explicit MqttClient(QObject *parent = nullptr);

    void connectToBroker(const QString& host, quint16 port = 1883, const QString& clientId = "QtHandmadeMQTT");
    void subscribe(const QString& topic, quint8 qos = 0);
    void publish(const QString& topic, const QByteArray& payload, quint8 qos = 0, bool retain = false);
    void disconnect();

private slots:
    void onSocketStateChanged(QAbstractSocket::SocketState state);
    void onReadyRead();
    void onPingTimerTimeout();

private:
    QByteArray encodeFixedHeader(MqttPacketType type, quint32 remainingLength);
    QByteArray encodeRemainingLength(quint32 length);
    QByteArray buildConnectPacket(const QString& clientId);
    QByteArray buildSubscribePacket(const QString& topic, quint8 qos, quint16 packetId);
    QByteArray buildPublishPacket(const QString& topic, const QByteArray& payload, quint8 qos, bool retain, quint16 packetId);
    QByteArray buildPingReqPacket();
    void parseConnAck(const QByteArray& data);
    void parseSubAck(const QByteArray& data);
    void parsePublish(const QByteArray& data);

private:
    QTcpSocket* m_socket;
    QTimer* m_pingTimer;
    QString m_clientId;
    quint16 m_nextPacketId;
    quint32 m_keepAlive;
};

#endif // MQTTCLIENT_H