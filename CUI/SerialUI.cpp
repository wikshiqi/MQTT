// SerialUI.cpp
#include "UI/SerialUI.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QDateTime>

SerialUI::SerialUI(QWidget *parent) : QWidget(parent)
{
    initUI();
    initStyle();
}

void SerialUI::initUI()
{
    // ========== 界面布局 ==========
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // 1. 串口配置
    QGroupBox* configGroup = new QGroupBox("串口配置", this);
    QGridLayout* configLayout = new QGridLayout(configGroup);

    // 串口选择
    configLayout->addWidget(new QLabel("串口："), 0, 0);
    m_portCombo = new QComboBox(this);
    m_refreshBtn = new QPushButton("刷新", this);
    configLayout->addWidget(m_portCombo, 0, 1);
    configLayout->addWidget(m_refreshBtn, 0, 2);

    // 波特率
    configLayout->addWidget(new QLabel("波特率："), 1, 0);
    m_baudRateCombo = new QComboBox(this);
    m_baudRateCombo->addItems({"9600", "19200", "38400", "115200"});
    m_baudRateCombo->setCurrentText("115200");
    configLayout->addWidget(m_baudRateCombo, 1, 1);

    // 数据位
    configLayout->addWidget(new QLabel("数据位："), 1, 2);
    m_dataBitsCombo = new QComboBox(this);
    m_dataBitsCombo->addItems({"8", "7", "6", "5"});
    m_dataBitsCombo->setCurrentText("8");
    configLayout->addWidget(m_dataBitsCombo, 1, 3);

    // 校验位
    configLayout->addWidget(new QLabel("校验位："), 2, 0);
    m_parityCombo = new QComboBox(this);
    m_parityCombo->addItems({"无", "奇校验", "偶校验"});
    configLayout->addWidget(m_parityCombo, 2, 1);

    // 停止位
    configLayout->addWidget(new QLabel("停止位："), 2, 2);
    m_stopBitsCombo = new QComboBox(this);
    m_stopBitsCombo->addItems({"1", "1.5", "2"});
    m_stopBitsCombo->setCurrentText("1");
    configLayout->addWidget(m_stopBitsCombo, 2, 3);

    // 打开/关闭按钮
    m_openBtn = new QPushButton("打开串口", this);
    configLayout->addWidget(m_openBtn, 3, 1, 1, 2);
    mainLayout->addWidget(configGroup);

    // 2. 发送区
    QGroupBox* sendGroup = new QGroupBox("发送区", this);
    QGridLayout* sendLayout = new QGridLayout(sendGroup);
    m_sendEdit = new QLineEdit(this);
    m_sendEdit->setPlaceholderText("输入要发送的数据");
    sendLayout->addWidget(m_sendEdit, 0, 0, 1, 2);
    m_sendBtn = new QPushButton("发送", this);
    m_sendBtn->setEnabled(false);
    sendLayout->addWidget(m_sendBtn, 0, 2);

    m_hexSendCheck = new QCheckBox("十六进制发送", this);
    sendLayout->addWidget(m_hexSendCheck, 1, 0);
    mainLayout->addWidget(sendGroup);

    // 3. 接收区
    QGroupBox* recvGroup = new QGroupBox("接收区", this);
    QVBoxLayout* recvLayout = new QVBoxLayout(recvGroup);
    m_hexRecvCheck = new QCheckBox("十六进制显示", this);
    recvLayout->addWidget(m_hexRecvCheck);
    m_recvEdit = new QTextEdit(this);
    m_recvEdit->setReadOnly(true);
    recvLayout->addWidget(m_recvEdit);
    mainLayout->addWidget(recvGroup);

    // ========== 初始化串口列表 ==========
    onRefreshPortClicked();

    // ========== 信号槽 ==========
    connect(m_refreshBtn, &QPushButton::clicked, this, &SerialModule::onRefreshPortClicked);
    connect(m_openBtn, &QPushButton::clicked, this, &SerialModule::onOpenPortClicked);
    connect(m_sendBtn, &QPushButton::clicked, this, &SerialModule::onSendDataClicked);
    connect(m_serial, &QSerialPort::readyRead, this, &SerialModule::onSerialReadyRead);
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialModule::onSerialErrorOccurred);
}

void SerialUI::initStyle()
{
    QFont font("Microsoft YaHei", 9);
    setFont(font);

    m_portCard->setAcrylicOpacity(0.85);
    m_baudRateCombo->setFixedHeight(32);
    m_dataBitsCombo->setFixedHeight(32);
    m_stopBitsCombo->setFixedHeight(32);
    m_parityCombo->setFixedHeight(32);

    m_receiveEdit->setStyleSheet(R"(
        ElaTextEdit {
            border-radius: 8px;
            padding: 8px;
            background-color: rgba(255,255,255,0.8);
        }
    )");
    m_sendEdit->setStyleSheet(m_receiveEdit->styleSheet());

    m_openBtn->setFixedSize(100, 35);
    m_closeBtn->setFixedSize(100, 35);
    m_sendBtn->setFixedSize(100, 35);
}

SerialConfig SerialUI::getSerialConfig() const
{
    SerialConfig config;
    config.portName = m_portCard->getUrlEditText();
    config.baudRate = m_baudRateCombo->currentText().toInt();
    config.dataBits = m_dataBitsCombo->currentText().toInt();

    // 停止位映射
    QString stopBit = m_stopBitsCombo->currentText();
    if (stopBit == "1") config.stopBits = QSerialPort::OneStop;
    else if (stopBit == "1.5") config.stopBits = QSerialPort::OneAndHalfStop;
    else config.stopBits = QSerialPort::TwoStop;

    // 校验位映射
    QString parity = m_parityCombo->currentText();
    if (parity == "无") config.parity = QSerialPort::NoParity;
    else if (parity == "奇") config.parity = QSerialPort::OddParity;
    else config.parity = QSerialPort::EvenParity;

    return config;
}

QByteArray SerialUI::getSendData() const
{
    return m_sendEdit->toPlainText().toUtf8();
}

void SerialUI::updateSerialState(bool isOpen)
{
    m_openBtn->setEnabled(!isOpen);
    m_closeBtn->setEnabled(isOpen);
    m_sendBtn->setEnabled(isOpen);
    m_portCard->setEnabled(!isOpen);
    m_baudRateCombo->setEnabled(!isOpen);
    m_dataBitsCombo->setEnabled(!isOpen);
    m_stopBitsCombo->setEnabled(!isOpen);
    m_parityCombo->setEnabled(!isOpen);
}

void SerialUI::appendReceivedData(const QString &data)
{
    m_receiveEdit->appendPlainText("[接收] " + QDateTime::currentDateTime().toString() + "：" + data);
}

void SerialUI::showError(const QString &error)
{
    m_receiveEdit->appendPlainText("[错误] " + QDateTime::currentDateTime().toString() + "：" + error);
}
void SerialUI::openSerialClicked(void) {
    emit openSerialPort(); // 触发打开串口信号
}

void SerialUI::closeSerialClicked(void) {
    emit closeSerialPort(); // 触发关闭串口信号
}

void SerialUI::sendDataClicked(void) {
    emit sendSerialData(getSendData()); // 触发发送数据信号
}
