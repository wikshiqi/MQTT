#include "MqttUI.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QDateTime>
#include <QGroupBox>

MqttUI::MqttUI(QWidget *parent) : QWidget(parent)
{
    initUI();
    initStyle();

    connect(m_connectBtn, &QPushButton::clicked, this, &MqttUI::connectMqttClicked);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &MqttUI::disconnectMqttClicked);
    connect(m_subscribeBtn, &QPushButton::clicked, this, &MqttUI::subscribeTopicClicked);
    connect(m_publishBtn, &QPushButton::clicked, this, &MqttUI::publishMsgClicked);
    connect(m_btnSwitch1, &QPushButton::clicked, this, &MqttUI::sendSwitch1d);
    connect(m_btnSwitch2, &QPushButton::clicked, this, &MqttUI::sendSwitch2d);
}

void MqttUI::initUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15,15,15,15);

    // 连接配置
    QGroupBox* connectGroup = new QGroupBox("OneNET MQTT配置", this);
    QGridLayout* connectLayout = new QGridLayout(connectGroup);
    connectLayout->addWidget(new QLabel("Broker地址："), 0,0);
    m_hostEdit = new QLineEdit("183.230.40.96");
    connectLayout->addWidget(m_hostEdit,0,1);
    connectLayout->addWidget(new QLabel("端口："),0,2);
    m_portEdit = new QLineEdit("1883");
    connectLayout->addWidget(m_portEdit,0,3);

    connectLayout->addWidget(new QLabel("ClientID："),1,0);
    m_clientIdEdit = new QLineEdit("DHT11");
    connectLayout->addWidget(m_clientIdEdit,1,1);
    connectLayout->addWidget(new QLabel("产品ID："),1,2);
    m_usernameEdit = new QLineEdit;
    m_usernameEdit->setText("5Tgf5AGpeZ");
    connectLayout->addWidget(m_usernameEdit,1,3);

    connectLayout->addWidget(new QLabel("APIKey："),2,0);
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setText("version=2018-10-31&res=products%2F5Tgf5AGpeZ%2Fdevices%2FDHT11&et=2076749615&method=md5&sign=c6mHqy5NX1d2%2B1s9carmhw%3D%3D");
    connectLayout->addWidget(m_passwordEdit,2,1,1,3);

    m_connectBtn = new QPushButton("连接");
    m_disconnectBtn = new QPushButton("断开");
    m_disconnectBtn->setEnabled(false);
    connectLayout->addWidget(m_connectBtn,3,1);
    connectLayout->addWidget(m_disconnectBtn,3,2);
    mainLayout->addWidget(connectGroup);

    // 订阅发布
    QGroupBox* pubSubGroup = new QGroupBox("订阅/发送消息", this);
    QGridLayout* pubSubLayout = new QGridLayout(pubSubGroup);
    pubSubLayout->addWidget(new QLabel("主题："),0,0);
    m_topicEdit = new QLineEdit;
    m_topicEdit->setText("$/sys/5Tgf5AGpeZ/DHT11/thing/event/property/post");
    pubSubLayout->addWidget(m_topicEdit,0,1);
    m_subscribeBtn = new QPushButton("订阅");
    pubSubLayout->addWidget(m_subscribeBtn,0,2);

    pubSubLayout->addWidget(new QLabel("消息："),1,0);
    m_payloadEdit = new QLineEdit("{\"temp\":25.5}");
    pubSubLayout->addWidget(m_payloadEdit,1,1);
    m_publishBtn = new QPushButton("发布");
    pubSubLayout->addWidget(m_publishBtn,1,2);
    mainLayout->addWidget(pubSubGroup);

    // 远程控制区
    QGroupBox* controlGroup = new QGroupBox("下位机控制", this);
    QGridLayout* controlLayout = new QGridLayout(controlGroup);
    m_btnSwitch1 = new QPushButton("开关1");
    m_btnSwitch2 = new QPushButton("开关2");
    m_btnSwitch1->setStyleSheet("background:#5a8dee; color:white;");
    m_btnSwitch2->setStyleSheet("background:#5a8dee; color:white;");
    controlLayout->addWidget(m_btnSwitch1,0,0);
    controlLayout->addWidget(m_btnSwitch2,0,1);
    mainLayout->addWidget(controlGroup);

    // 日志
    QGroupBox* logGroup = new QGroupBox("日志", this);
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);
    m_logEdit = new QTextEdit;
    m_logEdit->setReadOnly(true);
    logLayout->addWidget(m_logEdit);
    mainLayout->addWidget(logGroup);
}

void MqttUI::initStyle()
{
    QFont f("Microsoft YaHei",9);
    setFont(f);

    m_hostEdit->setFixedHeight(32);
    m_portEdit->setFixedHeight(32);
    m_clientIdEdit->setFixedHeight(32);
    m_usernameEdit->setFixedHeight(32);
    m_passwordEdit->setFixedHeight(32);
    m_topicEdit->setFixedHeight(32);
    m_payloadEdit->setFixedHeight(32);

    m_logEdit->setStyleSheet(R"(
        QTextEdit{border-radius:8px; padding:8px; background:rgba(255,255,255,0.8);}
    )");

    m_connectBtn->setFixedSize(100,35);
    m_disconnectBtn->setFixedSize(100,35);
    m_publishBtn->setFixedSize(80,35);
    m_subscribeBtn->setFixedSize(80,35);
    m_btnSwitch1->setFixedSize(100,35);
    m_btnSwitch2->setFixedSize(100,35);
}

MqttConfig MqttUI::getMqttConfig() const
{
    MqttConfig c;
    c.broker = m_hostEdit->text();
    c.port = m_portEdit->text().toInt();
    c.clientId = m_clientIdEdit->text();
    c.username = m_usernameEdit->text();
    c.password = m_passwordEdit->text();
    c.autoReconnect = true;
    return c;
}

QString MqttUI::getPublishTopic() const { return m_topicEdit->text(); }
QByteArray MqttUI::getPublishMsg() const { return m_payloadEdit->text().toUtf8(); }
QString MqttUI::getSubscribeTopic() const { return m_topicEdit->text(); }

void MqttUI::updateMqttState(bool en)
{
    m_connectBtn->setEnabled(!en);
    m_disconnectBtn->setEnabled(en);
    m_publishBtn->setEnabled(en);
    m_subscribeBtn->setEnabled(en);
    m_btnSwitch1->setEnabled(en);
    m_btnSwitch2->setEnabled(en);
}

void MqttUI::appendReceivedMsg(const QString &t, const QString &m)
{
    m_logEdit->append(QString("[%1] %2 -> %3").arg(QDateTime::currentDateTime().toString(), t, m));
}

void MqttUI::showError(const QString &e)
{
    m_logEdit->append("[错误] " + QDateTime::currentDateTime().toString() + "：" + e);
}

void MqttUI::showInfo(const QString &info)
{
    m_logEdit->append("[信息] " + QDateTime::currentDateTime().toString() + "：" + info);
}

void MqttUI::connectMqttClicked() { emit connectMqtt(); }
void MqttUI::disconnectMqttClicked() { emit disconnectMqtt(); }
void MqttUI::publishMsgClicked() { emit publishMessage(getPublishTopic(), getPublishMsg()); }
void MqttUI::subscribeTopicClicked() { emit subscribeTopic(getSubscribeTopic()); }
void MqttUI::sendSwitch1d() { emit sendSwitch1(); }
void MqttUI::sendSwitch2d() { emit sendSwitch2(); }