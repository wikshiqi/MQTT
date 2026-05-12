#include "mqttmodule.h"
#include "BackendClient.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QTimer>

MqttModule::MqttModule(MqttUI* ui, QWidget *parent)
        : QWidget(parent)
        , mqttUi(ui)
        , m_client(new QMqttClient()) // 注意这里加了 this 作为父对象
{


    // 绑定UI信号
    connect(mqttUi, &MqttUI::connectMqtt, this, &MqttModule::onConnectClicked);
    connect(mqttUi, &MqttUI::disconnectMqtt, this, &MqttModule::onDisconnectClicked);
    connect(mqttUi, &MqttUI::publishMessage, this, &MqttModule::onPublishClicked);
    connect(mqttUi, &MqttUI::subscribeTopic, this, &MqttModule::onSubscribeClicked);

    // 按钮绑定
    connect(mqttUi->m_connectBtn,    &QPushButton::clicked, this, &MqttModule::onConnectClicked);
    connect(mqttUi->m_disconnectBtn, &QPushButton::clicked, this, &MqttModule::onDisconnectClicked);
    connect(mqttUi->m_subscribeBtn,  &QPushButton::clicked, this, &MqttModule::onSubscribeClicked);
    connect(mqttUi->m_publishBtn,    &QPushButton::clicked, this, &MqttModule::onPublishClicked);

    // MQTT状态监听
    connect(m_client, &QMqttClient::connected,          this, &MqttModule::onClientConnected);
    connect(m_client, &QMqttClient::disconnected,       this, &MqttModule::onClientDisconnected);
    connect(m_client, &QMqttClient::messageReceived,    this, &MqttModule::onMessageReceived);
    connect(m_client, &QMqttClient::errorChanged,       this, &MqttModule::onClientError);

    // 绑定日志信号到UI
    connect(this, &MqttModule::logInfo, mqttUi, &MqttUI::showInfo);
    connect(this, &MqttModule::logError, mqttUi, &MqttUI::showError);

    // 关键配置（OneNET明文MQTT 3.1.1）
    m_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);
    m_client->setKeepAlive(60);
    m_client->setCleanSession(true);
}

MqttModule::~MqttModule()
{
    if (m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}

void MqttModule::onClientError(QMqttClient::ClientError error)
{
    QString errStr;
    switch (error) {
        case 0:   errStr = "无错误"; break;
        case 256: errStr = "协议版本错误"; break;
        case 257: errStr = "ClientID 格式错误"; break;
        case 258: errStr = "服务器不可用/连接超时"; break;
        case 259: errStr = "用户名或密码错误"; break;
        case 260: errStr = "未授权（设备/产品不匹配）"; break;
        default:  errStr = "未知错误码：" + QString::number(error);
    }
    emit logError("❌ 连接失败：" + errStr);
}

void MqttModule::onConnectClicked()
{
    auto config = mqttUi->getMqttConfig();

    emit logInfo("主机：" + config.broker);
    emit logInfo("端口：" + QString::number(config.port));
    emit logInfo("ClientID：" + config.clientId);
    emit logInfo("用户名：" + config.username);
    emit logInfo("用户名：" + config.password);
    // ✅ 正确设置（无任何编码问题）
    m_client->setHostname(config.broker);
    m_client->setPort(config.port);
    m_client->setClientId(config.clientId);
    m_client->setUsername(config.username);
    m_client->setPassword(config.password);

    m_client->connectToHost();
    emit logInfo("正在连接 OneNET...");


}

void MqttModule::onDisconnectClicked()
{
    m_client->disconnectFromHost();
    emit logInfo("手动断开连接");
}

void MqttModule::onSubscribeClicked()
{
    QString topic = mqttUi->m_topicEdit->text().trimmed();
    if (topic.isEmpty()) return;
    m_client->subscribe(QMqttTopicFilter(topic));
    emit logInfo("已订阅主题：" + topic);
}

void MqttModule::onPublishClicked()
{
    QString post_topic = mqttUi->m_topicEdit->text().trimmed();
    QByteArray payload = mqttUi->m_payloadEdit->text().toUtf8();
    if (post_topic.isEmpty() || payload.isEmpty()) return;

    m_client->publish(post_topic, payload);
    emit logInfo("发布消息：" + post_topic + " -> " + QString::fromUtf8(payload));
}

void MqttModule::onClientConnected()
{
    mqttUi->updateMqttState(true);
    emit logInfo("✅ OneNET 连接成功");
    QString subTopic = "$sys/5Tgf5AGpeZ/DHT11/thing/property/set";
    m_client->subscribe(subTopic);
    emit logInfo("✅ 已自动订阅：" + subTopic);
    subTopic="$sys/5Tgf5AGpeZ/DHT11/thing/property/post/reply";
    m_client->subscribe(subTopic);
    emit logInfo("✅ 已自动订阅：" + subTopic);
}

void MqttModule::onClientDisconnected()
{
    mqttUi->updateMqttState(false);
    emit logInfo("❌ 已断开连接");
}

void MqttModule::onMessageReceived(const QByteArray &payload, const QMqttTopicName &topic)
{
    QString topicStr = topic.name();
    QString payloadStr = QString::fromUtf8(payload);
    emit logInfo("📩 收到消息：" + topicStr + " -> " + payloadStr);

    QJsonParseError err{};
    QJsonDocument doc = QJsonDocument::fromJson(payload, &err);
    if (!err.error && doc.isObject()) {
        emit sendDataToChart(doc.object());
    }
    QJsonObject data = doc.object();
//    if (data.contains("temp") || data.contains("humi")) {
//        // 动态获取主窗口的后端客户端
//        if (parent() && parent()->parent()) {
//            auto mainWin = parent()->parent();
//            auto backend = mainWin->findChild<BackendClient*>();
//            if (backend) {
//                backend->uploadData("DHT11",
//                                    data.value("temp").toDouble(),
//                                    data.value("humi").toDouble());
//            }
//        }
//    }
    if (data.contains("params")) {
        QJsonObject params = data.value("params").toObject();

        double temperature = params.value("temperature").toDouble();
        double humidity    = params.value("humidity").toDouble();

        emit logInfo(QString("✅ 解析成功：温度=%1，湿度=%2").arg(temperature).arg(humidity));

        // 发送给图表
        QJsonObject chartData;
        chartData.insert("temp", temperature);
        chartData.insert("humi", humidity);
        emit sendDataToChart(chartData);

        // 发送给 SpringBoot 后端（真正执行！）
        if (parent() && parent()->parent()) {
            auto mainWin = parent()->parent();
            auto backend = mainWin->findChild<BackendClient*>();
            if (backend) {
                backend->uploadData("DHT11", temperature, humidity);
                emit logInfo("✅ 已推送给 SpringBoot 后端！");
            } else {
                emit logError("❌ 找不到 BackendClient，无法上传");
            }
        }
    }



}