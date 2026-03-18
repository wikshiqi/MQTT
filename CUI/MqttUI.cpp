
// MqttUI.cpp
#include "UI/MqttUI.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QDateTime>

MqttUI::MqttUI(QWidget *parent) : QWidget(parent)
{
    initUI();
    initStyle();
}

void MqttUI::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(12);

    // Broker地址
    m_brokerCard = new ElaAcrylicUrlCard(this);
    m_brokerCard->setUrlLabel("Broker地址");
    m_brokerCard->setUrlEditPlaceholder("tcp://127.0.0.1");
    mainLayout->addWidget(m_brokerCard);

    // 基础参数
    QGridLayout* baseLayout = new QGridLayout();
    baseLayout->addWidget(new QLabel("端口："), 0, 0);
    m_portCombo = new ElaComboBox(this);
    m_portCombo->addItems({"1883", "8883", "8083"});
    m_portCombo->setCurrentText("1883");
    baseLayout->addWidget(m_portCombo, 0, 1);

    baseLayout->addWidget(new QLabel("客户端ID："), 0, 2);
    m_clientIdEdit = new ElaLineEdit(this);
    m_clientIdEdit->setPlaceholderText("client_001");
    baseLayout->addWidget(m_clientIdEdit, 0, 3);

    m_autoReconnectCheck = new ElaCheckBox("自动重连", this);
    m_autoReconnectCheck->setChecked(true);
    baseLayout->addWidget(m_autoReconnectCheck, 1, 0);

    baseLayout->addWidget(new QLabel("用户名："), 1, 1);
    m_usernameEdit = new ElaLineEdit(this);
    m_usernameEdit->setPlaceholderText("可选");
    baseLayout->addWidget(m_usernameEdit, 1, 2);

    baseLayout->addWidget(new QLabel("密码："), 1, 3);
    m_passwordEdit = new ElaLineEdit(this);
    m_passwordEdit->setEchoMode(ElaLineEdit::Password);
    m_passwordEdit->setPlaceholderText("可选");
    baseLayout->addWidget(m_passwordEdit, 1, 4);
    mainLayout->addLayout(baseLayout);

    // 连接按钮
    QHBoxLayout* connectLayout = new QHBoxLayout();
    m_connectBtn = new QPushButton(this);
    m_connectBtn->setIcon(ElaIconType::Connect);
    m_connectBtn->setText("连接MQTT");
    m_connectBtn->setTheme(ElaThemeType::Primary);

    m_disconnectBtn = new QPushButton(this);
    m_disconnectBtn->setIcon(ElaIconType::Disconnect);
    m_disconnectBtn->setText("断开连接");
    m_disconnectBtn->setTheme(ElaThemeType::Danger);
    m_disconnectBtn->setEnabled(false);

    connectLayout->addWidget(m_connectBtn);
    connectLayout->addWidget(m_disconnectBtn);
    mainLayout->addLayout(connectLayout);

    // 发布消息
    QHBoxLayout* publishLayout = new QHBoxLayout();
    publishLayout->addWidget(new QLabel("发布主题："));
    m_publishTopicEdit = new ElaLineEdit(this);
    m_publishTopicEdit->setPlaceholderText("test/topic");
    publishLayout->addWidget(m_publishTopicEdit);

    m_publishBtn = new QPushButton(this);
    m_publishBtn->setIcon(ElaIconType::Send);
    m_publishBtn->setText("发布");
    m_publishBtn->setTheme(ElaThemeType::Success);
    m_publishBtn->setEnabled(false);
    publishLayout->addWidget(m_publishBtn);
    mainLayout->addLayout(publishLayout);

    m_publishMsgEdit = new QTextEdit(this);
    m_publishMsgEdit->setPlaceholderText("输入要发布的消息...");
    m_publishMsgEdit->setMaximumHeight(80);
    mainLayout->addWidget(m_publishMsgEdit);

    // 订阅主题
    QHBoxLayout* subscribeLayout = new QHBoxLayout();
    subscribeLayout->addWidget(new QLabel("订阅主题："));
    m_subscribeTopicEdit = new ElaLineEdit(this);
    m_subscribeTopicEdit->setPlaceholderText("test/topic");
    subscribeLayout->addWidget(m_subscribeTopicEdit);

    m_subscribeBtn = new QPushButton(this);
    m_subscribeBtn->setIcon(ElaIconType::Add);
    m_subscribeBtn->setText("订阅");
    m_subscribeBtn->setTheme(ElaThemeType::Info);
    m_subscribeBtn->setEnabled(false);
    subscribeLayout->addWidget(m_subscribeBtn);
    mainLayout->addLayout(subscribeLayout);

    // 接收消息
    m_receiveMsgEdit = new QTextEdit(this);
    m_receiveMsgEdit->setPlaceholderText("接收MQTT消息显示区...");
    m_receiveMsgEdit->setReadOnly(true);
    mainLayout->addWidget(m_receiveMsgEdit);

    // 绑定信号
    connect(m_connectBtn, &QPushButton::clicked, this, &MqttUI::connectMqttClicked);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &MqttUI::disconnectMqttClicked);
    connect(m_publishBtn, &QPushButton::clicked, this, &MqttUI::publishMsgClicked);
    connect(m_subscribeBtn, &QPushButton::clicked, this, &MqttUI::subscribeTopicClicked);
}

void MqttUI::initStyle()
{
    QFont font("Microsoft YaHei", 9);
    setFont(font);

    m_brokerCard->setAcrylicOpacity(0.85);
    m_portCombo->setFixedHeight(32);
    m_clientIdEdit->setFixedHeight(32);
    m_usernameEdit->setFixedHeight(32);
    m_passwordEdit->setFixedHeight(32);
    m_publishTopicEdit->setFixedHeight(32);
    m_subscribeTopicEdit->setFixedHeight(32);

    m_receiveMsgEdit->setStyleSheet(R"(
        QTextEdit {
            border-radius: 8px;
            padding: 8px;
            background-color: rgba(255,255,255,0.8);
        }
    )");
    m_publishMsgEdit->setStyleSheet(m_receiveMsgEdit->styleSheet());

    m_connectBtn->setFixedSize(100, 35);
    m_disconnectBtn->setFixedSize(100, 35);
    m_publishBtn->setFixedSize(80, 35);
    m_subscribeBtn->setFixedSize(80, 35);
}

MqttConfig MqttUI::getMqttConfig() const
{
    MqttConfig config;
    config.broker = m_brokerCard->getUrlEditText();
    config.port = m_portCombo->currentText().toInt();
    config.clientId = m_clientIdEdit->text();
    config.autoReconnect = m_autoReconnectCheck->isChecked();
    config.username = m_usernameEdit->text();
    config.password = m_passwordEdit->text();
    return config;
}

QString MqttUI::getPublishTopic() const
{
    return m_publishTopicEdit->text();
}

QByteArray MqttUI::getPublishMsg() const
{
    return m_publishMsgEdit->toPlainText().toUtf8();
}

QString MqttUI::getSubscribeTopic() const
{
    return m_subscribeTopicEdit->text();
}

void MqttUI::updateMqttState(bool isConnected)
{
    m_connectBtn->setEnabled(!isConnected);
    m_disconnectBtn->setEnabled(isConnected);
    m_publishBtn->setEnabled(isConnected);
    m_subscribeBtn->setEnabled(isConnected);

    m_brokerCard->setEnabled(!isConnected);
    m_portCombo->setEnabled(!isConnected);
    m_clientIdEdit->setEnabled(!isConnected);
    m_usernameEdit->setEnabled(!isConnected);
    m_passwordEdit->setEnabled(!isConnected);
}

void MqttUI::appendReceivedMsg(const QString &topic, const QString &msg)
{
    m_receiveMsgEdit->appendPlainText(QString("[%1] %2 -> %3").arg(
            QDateTime::currentDateTime().toString(), topic, msg
    ));
}

void MqttUI::showError(const QString &error)
{
    m_receiveMsgEdit->appendPlainText("[错误] " + QDateTime::currentDateTime().toString() + "：" + error);
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
    // 触发订阅信号（需先在头文件声明subscribeTopic信号）
    emit subscribeTopic(getSubscribeTopic());
}