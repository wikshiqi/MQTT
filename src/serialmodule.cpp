#include "serialmodule.h"
#include <QDateTime>
#include <QMessageBox>
#include <QRegExp>

SerialModule::SerialModule(QWidget *parent) : QWidget(parent)
        , m_serial(new QSerialPort(this))
        , m_isPortOpen(false)
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

SerialModule::~SerialModule() = default;

// 刷新串口列表
void SerialModule::onRefreshPortClicked()
{
    m_portCombo->clear();
            foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
            m_portCombo->addItem(info.portName() + " - " + info.description());
        }
    emit logInfo(QString("[%1] 刷新串口列表，共%2个串口").arg(QDateTime::currentDateTime().toString()).arg(m_portCombo->count()));
}

// 打开/关闭串口
void SerialModule::onOpenPortClicked()
{
    if (!m_isPortOpen) {
        // 打开串口
        if (m_portCombo->currentIndex() < 0) {
            QMessageBox::warning(this, "错误", "请选择串口！");
            return;
        }
        QString portName = m_portCombo->currentText().split(" - ").first();
        m_serial->setPortName(portName);

        // 配置串口参数
        m_serial->setBaudRate(m_baudRateCombo->currentText().toInt());
        m_serial->setDataBits(static_cast<QSerialPort::DataBits>(m_dataBitsCombo->currentText().toInt()));
        m_serial->setStopBits(m_stopBitsCombo->currentText() == "1" ? QSerialPort::OneStop :
                              (m_stopBitsCombo->currentText() == "1.5" ? QSerialPort::OneAndHalfStop : QSerialPort::TwoStop));
        m_serial->setParity(m_parityCombo->currentText() == "无" ? QSerialPort::NoParity :
                            (m_parityCombo->currentText() == "奇校验" ? QSerialPort::OddParity : QSerialPort::EvenParity));
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        // 打开串口
        if (m_serial->open(QIODevice::ReadWrite)) {
            m_isPortOpen = true;
            m_openBtn->setText("关闭串口");
            m_sendBtn->setEnabled(true);
            m_refreshBtn->setEnabled(false);
            emit logInfo(QString("[%1] 打开串口成功：%2").arg(QDateTime::currentDateTime().toString()).arg(portName));
        } else {
            emit logError(QString("[%1] 打开串口失败：%2").arg(QDateTime::currentDateTime().toString()).arg(m_serial->errorString()));
        }
    } else {
        // 关闭串口
        m_serial->close();
        m_isPortOpen = false;
        m_openBtn->setText("打开串口");
        m_sendBtn->setEnabled(false);
        m_refreshBtn->setEnabled(true);
        emit logInfo(QString("[%1] 关闭串口成功").arg(QDateTime::currentDateTime().toString()));
    }
}

// 发送数据
void SerialModule::onSendDataClicked()
{
    QString sendStr = m_sendEdit->text().trimmed();
    if (sendStr.isEmpty()) return;

    QByteArray sendData;
    if (m_hexSendCheck->isChecked()) {
        sendData = hexToByteArray(sendStr);
        if (sendData.isEmpty()) {
            QMessageBox::warning(this, "错误", "十六进制格式错误！");
            return;
        }
        emit logInfo(QString("[%1] 发送（十六进制）：%2").arg(QDateTime::currentDateTime().toString()).arg(sendStr));
    } else {
        sendData = sendStr.toUtf8();
        emit logInfo(QString("[%1] 发送：%2").arg(QDateTime::currentDateTime().toString()).arg(sendStr));
    }

    m_serial->write(sendData);
}

// 读取串口数据
void SerialModule::onSerialReadyRead()
{
    QByteArray recvData = m_serial->readAll();
    if (recvData.isEmpty()) return;

    QString recvStr;
    if (m_hexRecvCheck->isChecked()) {
        recvStr = byteArrayToHex(recvData);
        emit logInfo(QString("[%1] 接收（十六进制）：%2").arg(QDateTime::currentDateTime().toString()).arg(recvStr));
    } else {
        recvStr = QString::fromUtf8(recvData);
        emit logInfo(QString("[%1] 接收：%2").arg(QDateTime::currentDateTime().toString()).arg(recvStr));
    }

    m_recvEdit->append(recvStr);
}

// 串口错误处理
void SerialModule::onSerialErrorOccurred(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        emit logError(QString("[%1] 串口错误：%2").arg(QDateTime::currentDateTime().toString()).arg(m_serial->errorString()));
        // 自动关闭串口
        if (m_isPortOpen) {
            m_serial->close();
            m_isPortOpen = false;
            m_openBtn->setText("打开串口");
            m_sendBtn->setEnabled(false);
            m_refreshBtn->setEnabled(true);
        }
    }
}

// 字节数组转十六进制字符串
QString SerialModule::byteArrayToHex(const QByteArray& data)
{
    QString hexStr;
    for (char c : data) {
        hexStr += QString("%1 ").arg(static_cast<unsigned char>(c), 2, 16, QChar('0')).toUpper();
    }
    return hexStr.trimmed();
}

// 十六进制字符串转字节数组
// 十六进制字符串转字节数组（适配Qt5.14.2，解决const和QRegExp兼容问题）
QByteArray SerialModule::hexToByteArray(const QString& hex)
{
    // 1. 先拷贝const字符串到可修改的临时变量（核心修复）
    QString cleanHex = hex;
    // 2. 替换所有空白字符（改用QString::simplified + 移除空格，避免QRegExp的坑）
    cleanHex = cleanHex.simplified().replace(" ", "").toUpper();
    QByteArray data;

    // 3. 边界检查：确保字符数是偶数（十六进制必须2个字符一组）
    if (cleanHex.length() % 2 != 0) {
        qWarning() << "十六进制字符串长度必须为偶数！当前长度：" << cleanHex.length();
        return QByteArray();
    }

    // 4. 逐字节转换
    for (int i = 0; i < cleanHex.length(); i += 2) {
        QString byteStr = cleanHex.mid(i, 2);
        bool ok;
        // 5. 改用quint8避免char符号问题，再强制转换（Qt5.14更安全）
        quint8 byte = static_cast<quint8>(byteStr.toInt(&ok, 16));
        if (!ok) {
            qWarning() << "十六进制转换失败：" << byteStr;
            return QByteArray();
        }
        data.append(static_cast<char>(byte));
    }
    return data;
}