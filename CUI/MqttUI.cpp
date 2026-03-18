#include "UI/MqttUI.h"
// 补充缺失的Qt头文件
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QDateTime>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QCheckBox>

MqttUI::MqttUI(QWidget *parent) : QWidget(parent)
{
    initUI();
    initStyle();
    // 绑定按钮点击信号
    connect(m_connectBtn, &QPushButton::clicked, this, &MqttUI::connectMqttClicked);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &MqttUI::disconnectMqttClicked);
    connect(m_subscribeBtn, &QPushButton::clicked, this, &MqttUI::subscribeTopicClicked);
    connect(m_publishBtn, &QPushButton::clicked, this, &MqttUI::publishMsgClicked);
}

void MqttUI::initUI()
{
    // ========== 界面布局 ==========
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // 1. 连接配置
    QGroupBox* connectGroup = new QGroupBox("OneNET MQTT配置", this);
    QGridLayout* connectLayout = new QGridLayout(connectGroup);
    connectLayout->addWidget(new QLabel("Broker地址："), 0, 0);
    m_hostEdit = new QLineEdit("mqtts.heclouds.com", this);
    connectLayout->addWidget(m_hostEdit, 0, 1);
    connectLayout->addWidget(new QLabel("端口："), 0, 2);
    m_portEdit = new QLineEdit("1883", this);
    connectLayout->addWidget(m_portEdit, 0, 3);

    connectLayout->addWidget(new QLabel("ClientID："), 1, 0);
    m_clientIdEdit = new QLineEdit("DHT11", this);
    connectLayout->addWidget(m_clientIdEdit, 1, 1);
    connectLayout->addWidget(new QLabel("产品ID："), 1, 2);
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("OneNET产品ID");
    m_usernameEdit->setText("5Tgf5AGpeZ");
    connectLayout->addWidget(m_usernameEdit, 1, 3);

    connectLayout->addWidget(new QLabel("APIKey："), 2, 0);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("OneNET设备APIKey");
    m_passwordEdit->setText("version=2018-10-31&res=products%2F5Tgf5AGpeZ%2Fdevices%2FDHT11&et=2076749615&method=md5&sign=c6mHqy5NX1d2%2B1s9carmhw%3D%3D");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    connectLayout->addWidget(m_passwordEdit, 2, 1, 1, 3);

    m_connectBtn = new QPushButton("连接", this);
    m_disconnectBtn = new QPushButton("断开", this);
    m_disconnectBtn->setEnabled(false);
    connectLayout->addWidget(m_connectBtn, 3, 1);
    connectLayout->addWidget(m_disconnectBtn, 3, 2);
    mainLayout->addWidget(connectGroup);

    // 2. 订阅/发布
    QGroupBox* pubSubGroup = new QGroupBox("订阅/发布", this);
    QGridLayout* pubSubLayout = new QGridLayout(pubSubGroup);
    pubSubLayout->addWidget(new QLabel("主题："), 0, 0);
    m_topicEdit = new QLineEdit(this);
    m_topicEdit->setPlaceholderText("/sys/产品ID/设备ID/thing/event/property/post");
    m_topicEdit->setText("$sys/5Tgf5AGpeZ/DHT11/thing/property/post/reply");
    pubSubLayout->addWidget(m_topicEdit, 0, 1);
    m_subscribeBtn = new QPushButton("订阅", this);
    m_subscribeBtn->setEnabled(false);
    pubSubLayout->addWidget(m_subscribeBtn, 0, 2);

    pubSubLayout->addWidget(new QLabel("消息："), 1, 0);
    m_payloadEdit = new QLineEdit(this);
    m_payloadEdit->setPlaceholderText("{\"temp\":25.5}");
    pubSubLayout->addWidget(m_payloadEdit, 1, 1);
    m_publishBtn = new QPushButton("发布", this);
    m_publishBtn->setEnabled(false);
    pubSubLayout->addWidget(m_publishBtn, 1, 2);
    mainLayout->addWidget(pubSubGroup);

    // 3. 日志显示
    QGroupBox* logGroup = new QGroupBox("日志", this);
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);
    m_logEdit = new QTextEdit(this);
    m_logEdit->setReadOnly(true);
    logLayout->addWidget(m_logEdit);
    mainLayout->addWidget(logGroup);
}

void MqttUI::initStyle()
{
    QFont font("Microsoft YaHei", 9);
    setFont(font);

    // 修复：移除不存在的m_brokerCard（改用实际定义的变量）
    m_hostEdit->setFixedHeight(32);
    m_portEdit->setFixedHeight(32);
    m_clientIdEdit->setFixedHeight(32);
    m_usernameEdit->setFixedHeight(32);
    m_passwordEdit->setFixedHeight(32);
    m_topicEdit->setFixedHeight(32);
    m_payloadEdit->setFixedHeight(32);

    // 修复：QTextEdit正确的样式设置（移除ElaTextEdit，改用原生Qt样式）
    m_logEdit->setStyleSheet(R"(
        QTextEdit {
            border-radius: 8px;
            padding: 8px;
            background-color: rgba(255,255,255,0.8);
        }
    )");

    m_connectBtn->setFixedSize(100, 35);
    m_disconnectBtn->setFixedSize(100, 35);
    m_publishBtn->setFixedSize(80, 35);
    m_subscribeBtn->setFixedSize(80, 35);
}

MqttConfig MqttUI::getMqttConfig() const
{
    MqttConfig config;
    // 修复：改用实际定义的m_hostEdit，移除不存在的m_brokerCard/m_portCombo
    config.broker = m_hostEdit->text();
    config.port = m_portEdit->text().toInt();
    config.clientId = m_clientIdEdit->text();
    config.autoReconnect = true; // 简化：默认开启自动重连
    config.username = m_usernameEdit->text();
    config.password = m_passwordEdit->text();
    return config;
}

QString MqttUI::getPublishTopic() const
{
    // 修复：改用实际定义的m_topicEdit
    return m_topicEdit->text();
}

QByteArray MqttUI::getPublishMsg() const
{
    // 修复：改用实际定义的m_payloadEdit
    return m_payloadEdit->text().toUtf8();
}

QString MqttUI::getSubscribeTopic() const
{
    // 修复：改用实际定义的m_topicEdit
    return m_topicEdit->text();
}

void MqttUI::updateMqttState(bool isConnected)
{
    m_connectBtn->setEnabled(!isConnected);
    m_disconnectBtn->setEnabled(isConnected);
    m_publishBtn->setEnabled(isConnected);
    m_subscribeBtn->setEnabled(isConnected);

    // 修复：移除不存在的m_brokerCard/m_portCombo
    m_hostEdit->setEnabled(!isConnected);
    m_portEdit->setEnabled(!isConnected);
    m_clientIdEdit->setEnabled(!isConnected);
    m_usernameEdit->setEnabled(!isConnected);
    m_passwordEdit->setEnabled(!isConnected);
}

void MqttUI::appendReceivedMsg(const QString &topic, const QString &msg)
{
    // 修复：QTextEdit没有appendPlainText，改用append
    m_logEdit->append(QString("[%1] %2 -> %3").arg(
            QDateTime::currentDateTime().toString(), topic, msg
    ));
}

void MqttUI::showError(const QString &error)
{
    // 修复：QTextEdit正确的错误日志输出
    m_logEdit->append("[错误] " + QDateTime::currentDateTime().toString() + "：" + error);
}

void MqttUI::connectMqttClicked(void) {
    emit connectMqtt(); // 触发连接信号
}

void MqttUI::disconnectMqttClicked(void) {
    emit disconnectMqtt(); // 触发断开信号
}

void MqttUI::publishMsgClicked(void) {
    emit publishMessage(getPublishTopic(), getPublishMsg()); // 触发发布信号
}

void MqttUI::subscribeTopicClicked(void) {
    emit subscribeTopic(getSubscribeTopic()); // 触发订阅信号
}