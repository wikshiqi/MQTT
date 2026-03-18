#include "mqttmodule.h"
#include <QDateTime>
#include <QMessageBox>

MqttModule::MqttModule(MqttUI* ui, QWidget *parent) : QWidget(parent)
, mqttUi(ui) // 初始化UI指针
, m_socket(new QTcpSocket(this))
, m_pingTimer(new QTimer(this))
, m_nextPacketId(1)
, m_keepAlive(60)
{
    // ========== 信号槽 ==========
    // 修复：使用初始化后的m_mqttUi指针
    connect(mqttUi->m_connectBtn, &QPushButton::clicked, this, &MqttModule::onConnectClicked);
    connect(mqttUi->m_disconnectBtn, &QPushButton::clicked, this, &MqttModule::onDisconnectClicked);
    connect(mqttUi->m_subscribeBtn, &QPushButton::clicked, this, &MqttModule::onSubscribeClicked);
    connect(mqttUi->m_publishBtn, &QPushButton::clicked, this, &MqttModule::onPublishClicked);
    connect(m_socket, &QTcpSocket::stateChanged, this, &MqttModule::onSocketStateChanged);
    connect(m_socket, &QTcpSocket::readyRead, this, &MqttModule::onReadyRead);
    connect(m_pingTimer, &QTimer::timeout, this, &MqttModule::onPingTimerTimeout);
}

MqttModule::~MqttModule() = default;

// 连接OneNET
void MqttModule::onConnectClicked()
{
    QString host = mqttUi->m_hostEdit->text().trimmed();
    quint16 port = mqttUi->m_portEdit->text().toUShort();
    QString clientId = mqttUi->m_clientIdEdit->text().trimmed();
    QString username = mqttUi->m_usernameEdit->text().trimmed();
    QString password = mqttUi->m_passwordEdit->text().trimmed();

    if (host.isEmpty() || username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "错误", "请填写完整配置！");
        return;
    }

    m_socket->connectToHost(host, port);
    emit logInfo(QString("[%1] 正在连接%2:%3...").arg(QDateTime::currentDateTime().toString()).arg(host).arg(port));
}

// 断开连接
void MqttModule::onDisconnectClicked()
{
    m_socket->disconnectFromHost();
    m_pingTimer->stop();
    emit logInfo(QString("[%1] 断开连接").arg(QDateTime::currentDateTime().toString()));
}

// 订阅主题
void MqttModule::onSubscribeClicked()
{
    QString topic = mqttUi->m_topicEdit->text().trimmed();
    if (topic.isEmpty()) {
        QMessageBox::warning(this, "错误", "请填写订阅主题！");
        return;
    }
    QByteArray subPacket = buildSubscribePacket(topic, 0, m_nextPacketId++);
    m_socket->write(subPacket);
    emit logInfo(QString("[%1] 订阅主题：%2").arg(QDateTime::currentDateTime().toString()).arg(topic));
}

// 发布消息
void MqttModule::onPublishClicked()
{
    QString topic = post_topic;
    QByteArray payload = mqttUi->m_payloadEdit->text().toUtf8();
    if (topic.isEmpty() || payload.isEmpty()) {
        QMessageBox::warning(this, "错误", "请填写主题和消息！");
        return;
    }
    QByteArray pubPacket = buildPublishPacket(topic, payload, 0, false, m_nextPacketId++);
    m_socket->write(pubPacket);
    emit logInfo(QString("[%1] 发布消息：%2 -> %3").arg(QDateTime::currentDateTime().toString()).arg(topic).arg(QString(payload)));
}

// TCP状态变化
void MqttModule::onSocketStateChanged(QAbstractSocket::SocketState state)
{
    if (state == QTcpSocket::ConnectedState) {
        mqttUi->m_connectBtn->setEnabled(false);
        mqttUi->m_disconnectBtn->setEnabled(true);
        mqttUi->m_subscribeBtn->setEnabled(true);
        mqttUi->m_publishBtn->setEnabled(true);
        // 发送CONNECT报文（带OneNET认证）
        m_socket->write(buildConnectPacket(mqttUi->m_clientIdEdit->text(), mqttUi->m_usernameEdit->text(), mqttUi->m_passwordEdit->text()));
        emit logInfo(QString("[%1] TCP连接成功，发送认证报文").arg(QDateTime::currentDateTime().toString()));
    } else if (state == QTcpSocket::UnconnectedState) {
        mqttUi->m_connectBtn->setEnabled(true);
        mqttUi->m_disconnectBtn->setEnabled(false);
        mqttUi->m_subscribeBtn->setEnabled(false);
        mqttUi->m_publishBtn->setEnabled(false);
        emit logInfo(QString("[%1] TCP断开连接").arg(QDateTime::currentDateTime().toString()));
    }
}

// 读取MQTT报文
void MqttModule::onReadyRead()
{
    QByteArray data = m_socket->readAll();
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
        case CONNACK: parseConnAck(payloadData); break;
        case SUBACK: parseSubAck(payloadData); break;
        case PUBLISH: parsePublish(payloadData); break;
        case PINGRESP: emit logInfo(QString("[%1] 收到心跳响应").arg(QDateTime::currentDateTime().toString())); break;
        default: emit logInfo(QString("[%1] 未处理报文类型：%2").arg(QDateTime::currentDateTime().toString()).arg(static_cast<int>(type)));
    }
}

// 心跳定时器
void MqttModule::onPingTimerTimeout()
{
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        QByteArray pingPacket = encodeFixedHeader(PINGREQ, 0);
        m_socket->write(pingPacket);
        emit logInfo(QString("[%1] 发送心跳").arg(QDateTime::currentDateTime().toString()));
    }
}

// ========== MQTT协议封装 ==========
QByteArray MqttModule::encodeFixedHeader(MqttPacketType type, quint32 remainingLength)
{
    QByteArray fixedHeader;
    quint8 firstByte = (static_cast<quint8>(type) << 4) | 0x00;
    fixedHeader.append(static_cast<char>(firstByte));
    fixedHeader.append(encodeRemainingLength(remainingLength));
    return fixedHeader;
}

QByteArray MqttModule::encodeRemainingLength(quint32 length)
{
    QByteArray encoded;
    do {
        quint8 byte = length % 128;
        length /= 128;
        if (length > 0) byte |= 0x80;
        encoded.append(static_cast<char>(byte));
    } while (length > 0);
    return encoded;
}

QByteArray MqttModule::buildConnectPacket(const QString& clientId, const QString& username, const QString& password)
{
    QByteArray variableHeader;
    variableHeader.append(static_cast<char>(0x00)).append(static_cast<char>(0x04)).append("MQTT");
    variableHeader.append(static_cast<char>(0x04));
    quint8 connectFlags = 0x02 | 0x80 | 0x40; // CleanSession + 用户名 + 密码
    variableHeader.append(static_cast<char>(connectFlags));
    variableHeader.append(static_cast<char>(m_keepAlive >> 8)).append(static_cast<char>(m_keepAlive & 0xFF));

    QByteArray payload;
    quint16 len = clientId.length();
    payload.append(static_cast<char>(len >> 8)).append(static_cast<char>(len & 0xFF)).append(clientId.toUtf8());
    len = username.length();
    payload.append(static_cast<char>(len >> 8)).append(static_cast<char>(len & 0xFF)).append(username.toUtf8());
    len = password.length();
    payload.append(static_cast<char>(len >> 8)).append(static_cast<char>(len & 0xFF)).append(password.toUtf8());

    QByteArray packet = encodeFixedHeader(CONNECT, variableHeader.length() + payload.length());
    packet.append(variableHeader).append(payload);
    return packet;
}

QByteArray MqttModule::buildSubscribePacket(const QString& topic, quint8 qos, quint16 packetId)
{
    QByteArray variableHeader;
    variableHeader.append(static_cast<char>(packetId >> 8)).append(static_cast<char>(packetId & 0xFF));
    QByteArray payload;
    quint16 len = topic.length();
    payload.append(static_cast<char>(len >> 8)).append(static_cast<char>(len & 0xFF)).append(topic.toUtf8()).append(static_cast<char>(qos));
    QByteArray fixedHeader;
    fixedHeader.append(static_cast<char>((SUBSCRIBE << 4) | 0x02));
    fixedHeader.append(encodeRemainingLength(variableHeader.length() + payload.length()));
    return fixedHeader + variableHeader + payload;
}

QByteArray MqttModule::buildPublishPacket(const QString& topic, const QByteArray& payload, quint8 qos, bool retain, quint16 packetId)
{
    QByteArray variableHeader;
    quint16 len = topic.length();
    variableHeader.append(static_cast<char>(len >> 8)).append(static_cast<char>(len & 0xFF)).append(topic.toUtf8());
    if (qos > 0) variableHeader.append(static_cast<char>(packetId >> 8)).append(static_cast<char>(packetId & 0xFF));

    quint8 flags = (retain ? 0x01 : 0x00) | ((qos & 0x03) << 1);
    QByteArray fixedHeader;
    fixedHeader.append(static_cast<char>((PUBLISH << 4) | flags));
    fixedHeader.append(encodeRemainingLength(variableHeader.length() + payload.length()));
    return fixedHeader + variableHeader + payload;
}

// ========== MQTT协议解析 ==========
void MqttModule::parseConnAck(const QByteArray& data)
{
    quint8 returnCode = static_cast<quint8>(static_cast<unsigned char>(data.at(1)));
    if (returnCode == 0) {
        emit logInfo(QString("[%1] MQTT认证成功（OneNET连接成功）").arg(QDateTime::currentDateTime().toString()));
        m_pingTimer->start(m_keepAlive * 500);
    } else {
        emit logError(QString("[%1] MQTT认证失败，返回码：%2").arg(QDateTime::currentDateTime().toString()).arg(returnCode));
    }
}

void MqttModule::parseSubAck(const QByteArray& data)
{
    quint8 returnCode = static_cast<quint8>(static_cast<unsigned char>(data.at(2)));
    if (returnCode == 0) {
        emit logInfo(QString("[%1] 订阅成功").arg(QDateTime::currentDateTime().toString()));
    } else {
        emit logError(QString("[%1] 订阅失败，返回码：%2").arg(QDateTime::currentDateTime().toString()).arg(returnCode));
    }
}

void MqttModule::parsePublish(const QByteArray& data)
{
    quint16 topicLen = (static_cast<quint16>(static_cast<unsigned char>(data.at(0))) << 8) | static_cast<unsigned char>(data.at(1));
    QString topic = QString::fromUtf8(data.mid(2, topicLen));
    QString payload = QString::fromUtf8(data.mid(2 + topicLen));
    emit logInfo(QString("[%1] 收到消息：%2 -> %3").arg(QDateTime::currentDateTime().toString()).arg(topic).arg(payload));
}