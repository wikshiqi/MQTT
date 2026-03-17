#include "mqttclient.h"
#include <QDebug>

MqttClient::MqttClient(QObject *parent)
        : QObject(parent)
        , m_socket(new QTcpSocket(this))
        , m_pingTimer(new QTimer(this))
        , m_nextPacketId(1)
        , m_keepAlive(60)
{
    connect(m_socket, &QTcpSocket::stateChanged, this, &MqttClient::onSocketStateChanged);
    connect(m_socket, &QTcpSocket::readyRead, this, &MqttClient::onReadyRead);
    connect(m_pingTimer, &QTimer::timeout, this, &MqttClient::onPingTimerTimeout);
}

void MqttClient::connectToBroker(const QString &host, quint16 port, const QString &clientId)
{
    m_clientId = clientId;
    m_socket->connectToHost(host, port);
}

void MqttClient::subscribe(const QString &topic, quint8 qos)
{
    if (m_socket->state() != QTcpSocket::ConnectedState) {
        qWarning() << "TCP not connected, cannot subscribe";
        return;
    }
    QByteArray subPacket = buildSubscribePacket(topic, qos, m_nextPacketId++);
    m_socket->write(subPacket);
}

void MqttClient::publish(const QString &topic, const QByteArray &payload, quint8 qos, bool retain)
{
    if (m_socket->state() != QTcpSocket::ConnectedState) {
        qWarning() << "TCP not connected, cannot publish";
        return;
    }
    QByteArray pubPacket = buildPublishPacket(topic, payload, qos, retain, m_nextPacketId++);
    m_socket->write(pubPacket);
}

void MqttClient::disconnect()
{
    m_pingTimer->stop();
    m_socket->disconnectFromHost();
}

QByteArray MqttClient::encodeFixedHeader(MqttPacketType type, quint32 remainingLength)
{
    QByteArray fixedHeader;
    quint8 firstByte = (static_cast<quint8>(type) << 4) | 0x00;
    fixedHeader.append(static_cast<char>(firstByte));
    fixedHeader.append(encodeRemainingLength(remainingLength));
    return fixedHeader;
}

QByteArray MqttClient::encodeRemainingLength(quint32 length)
{
    QByteArray encoded;
    do {
        quint8 byte = length % 128;
        length /= 128;
        if (length > 0) {
            byte |= 0x80;
        }
        encoded.append(static_cast<char>(byte));
    } while (length > 0);
    return encoded;
}

QByteArray MqttClient::buildConnectPacket(const QString &clientId)
{
    QByteArray variableHeader;
    variableHeader.append(static_cast<char>(0x00));
    variableHeader.append(static_cast<char>(0x04));
    variableHeader.append("MQTT");
    variableHeader.append(static_cast<char>(0x04));
    quint8 connectFlags = 0x02;
    variableHeader.append(static_cast<char>(connectFlags));
    variableHeader.append(static_cast<char>(m_keepAlive >> 8));
    variableHeader.append(static_cast<char>(m_keepAlive & 0xFF));

    QByteArray payload;
    quint16 clientIdLen = clientId.length();
    payload.append(static_cast<char>(clientIdLen >> 8));
    payload.append(static_cast<char>(clientIdLen & 0xFF));
    payload.append(clientId.toUtf8());

    QByteArray connectPacket;
    quint32 remainingLength = variableHeader.length() + payload.length();
    connectPacket.append(encodeFixedHeader(CONNECT, remainingLength));
    connectPacket.append(variableHeader);
    connectPacket.append(payload);

    return connectPacket;
}

QByteArray MqttClient::buildSubscribePacket(const QString &topic, quint8 qos, quint16 packetId)
{
    QByteArray variableHeader;
    variableHeader.append(static_cast<char>(packetId >> 8));
    variableHeader.append(static_cast<char>(packetId & 0xFF));

    QByteArray payload;
    quint16 topicLen = topic.length();
    payload.append(static_cast<char>(topicLen >> 8));
    payload.append(static_cast<char>(topicLen & 0xFF));
    payload.append(topic.toUtf8());
    payload.append(static_cast<char>(qos));

    QByteArray fixedHeader;
    quint8 firstByte = (static_cast<quint8>(SUBSCRIBE) << 4) | 0x02;
    fixedHeader.append(static_cast<char>(firstByte));
    quint32 remainingLength = variableHeader.length() + payload.length();
    fixedHeader.append(encodeRemainingLength(remainingLength));

    return fixedHeader + variableHeader + payload;
}

QByteArray MqttClient::buildPublishPacket(const QString &topic, const QByteArray &payload, quint8 qos, bool retain, quint16 packetId)
{
    QByteArray variableHeader;
    quint16 topicLen = topic.length();
    variableHeader.append(static_cast<char>(topicLen >> 8));
    variableHeader.append(static_cast<char>(topicLen & 0xFF));
    variableHeader.append(topic.toUtf8());
    if (qos > 0) {
        variableHeader.append(static_cast<char>(packetId >> 8));
        variableHeader.append(static_cast<char>(packetId & 0xFF));
    }

    QByteArray fixedHeader;
    quint8 flags = 0;
    flags |= (retain ? 0x01 : 0x00);
    flags |= ((qos & 0x03) << 1);
    quint8 firstByte = (static_cast<quint8>(PUBLISH) << 4) | flags;
    fixedHeader.append(static_cast<char>(firstByte));
    quint32 remainingLength = variableHeader.length() + payload.length();
    fixedHeader.append(encodeRemainingLength(remainingLength));

    return fixedHeader + variableHeader + payload;
}

QByteArray MqttClient::buildPingReqPacket()
{
    return encodeFixedHeader(PINGREQ, 0);
}

void MqttClient::parseConnAck(const QByteArray &data)
{
    if (data.length() < 2) {
        qWarning() << "Invalid CONNACK packet";
        return;
    }
    quint8 connectAckFlags = static_cast<quint8>(static_cast<unsigned char>(data.at(0)));
    quint8 returnCode = static_cast<quint8>(static_cast<unsigned char>(data.at(1)));

    if (returnCode == 0) {
        qInfo() << "MQTT connected successfully!";
        m_pingTimer->start(m_keepAlive * 500);
    } else {
        qWarning() << "MQTT connect failed, return code:" << returnCode;
    }
}

void MqttClient::parseSubAck(const QByteArray &data)
{
    if (data.length() < 3) {
        qWarning() << "Invalid SUBACK packet";
        return;
    }
    quint16 packetId = (static_cast<quint16>(static_cast<unsigned char>(data.at(0))) << 8) |
                       static_cast<unsigned char>(data.at(1));
    quint8 returnCode = static_cast<quint8>(static_cast<unsigned char>(data.at(2)));
    if (returnCode == 0) {
        qInfo() << "Subscribe success, packet ID:" << packetId;
    } else {
        qWarning() << "Subscribe failed, return code:" << returnCode;
    }
}

void MqttClient::parsePublish(const QByteArray &data)
{
    quint16 topicLen = (static_cast<quint16>(static_cast<unsigned char>(data.at(0))) << 8) |
                       static_cast<unsigned char>(data.at(1));
    QString topic = QString::fromUtf8(data.mid(2, topicLen));
    int payloadStart = 2 + topicLen;
    QByteArray payload = data.mid(payloadStart);

    qInfo() << "Received message: topic=" << topic << ", payload=" << payload;
}

void MqttClient::onSocketStateChanged(QAbstractSocket::SocketState state)
{
    switch (state) {
        case QAbstractSocket::ConnectedState:
            qInfo() << "TCP connected, send CONNECT packet";
            m_socket->write(buildConnectPacket(m_clientId));
            break;
        case QAbstractSocket::UnconnectedState:
            qInfo() << "TCP disconnected";
            m_pingTimer->stop();
            break;
        default:
            qInfo() << "TCP state:" << state;
            break;
    }
}

void MqttClient::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    if (data.isEmpty()) return;

    quint8 firstByte = static_cast<quint8>(static_cast<unsigned char>(data.at(0)));
    MqttPacketType type = static_cast<MqttPacketType>((firstByte >> 4) & 0x0F);

    int remainingLenPos = 1;
    quint32 remainingLength = 0;
    quint8 byte;
    do {
        byte = static_cast<quint8>(static_cast<unsigned char>(data.at(remainingLenPos++)));
        remainingLength = (remainingLength << 7) | (byte & 0x7F);
    } while (byte & 0x80);

    QByteArray payloadData = data.mid(remainingLenPos, remainingLength);

    switch (type) {
        case CONNACK:
            parseConnAck(payloadData);
            break;
        case SUBACK:
            parseSubAck(payloadData);
            break;
        case PUBLISH:
            parsePublish(payloadData);
            break;
        case PINGRESP:
            qInfo() << "Received PINGRESP";
            break;
        default:
            qInfo() << "Unhandled packet type:" << static_cast<int>(type);
            break;
    }
}

void MqttClient::onPingTimerTimeout()
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        qInfo() << "Send PINGREQ";
        m_socket->write(buildPingReqPacket());
    }
}