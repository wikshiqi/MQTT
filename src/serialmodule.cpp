#include "serialmodule.h"
#include <QDateTime>
#include <QMessageBox>
//#include <QRegExp>

SerialModule::SerialModule(SerialUI*ui,QWidget *parent) : QWidget(parent)
        ,serialUi(ui)
        , m_serial(new QSerialPort(this))
        , m_isPortOpen(false)
{


    // ========== 初始化串口列表 ==========
    onRefreshPortClicked();

    // ========== 信号槽 ==========
    connect(serialUi->m_refreshBtn, &QPushButton::clicked, this, &SerialModule::onRefreshPortClicked);
    connect(serialUi->m_openBtn, &QPushButton::clicked, this, &SerialModule::onOpenPortClicked);
    connect(serialUi->m_sendBtn, &QPushButton::clicked, this, &SerialModule::onSendDataClicked);
    connect(m_serial, &QSerialPort::readyRead, this, &SerialModule::onSerialReadyRead);
    connect(m_serial, &QSerialPort::errorOccurred, this, &SerialModule::onSerialErrorOccurred);
}

SerialModule::~SerialModule() = default;

// 刷新串口列表
void SerialModule::onRefreshPortClicked()
{
    serialUi->m_portCombo->clear();
            foreach (const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
            serialUi->m_portCombo->addItem(info.portName() + " - " + info.description());
        }
    emit logInfo(QString("[%1] 刷新串口列表，共%2个串口").arg(QDateTime::currentDateTime().toString()).arg(serialUi->m_portCombo->count()));
}

// 打开/关闭串口
void SerialModule::onOpenPortClicked()
{
    if (!m_isPortOpen) {
        // 打开串口
        if (serialUi->m_portCombo->currentIndex() < 0) {
            QMessageBox::warning(this, "错误", "请选择串口！");
            return;
        }
        QString portName = serialUi->m_portCombo->currentText().split(" - ").first();
        m_serial->setPortName(portName);

        // 配置串口参数
        m_serial->setBaudRate(serialUi->m_baudRateCombo->currentText().toInt());
        m_serial->setDataBits(static_cast<QSerialPort::DataBits>(serialUi->m_dataBitsCombo->currentText().toInt()));
        m_serial->setStopBits(serialUi->m_stopBitsCombo->currentText() == "1" ? QSerialPort::OneStop :
                              (serialUi->m_stopBitsCombo->currentText() == "1.5" ? QSerialPort::OneAndHalfStop : QSerialPort::TwoStop));
        m_serial->setParity(serialUi->m_parityCombo->currentText() == "无" ? QSerialPort::NoParity :
                            (serialUi->m_parityCombo->currentText() == "奇校验" ? QSerialPort::OddParity : QSerialPort::EvenParity));
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        // 打开串口
        if (m_serial->open(QIODevice::ReadWrite)) {
            m_isPortOpen = true;
            serialUi->m_openBtn->setText("关闭串口");
            serialUi->m_sendBtn->setEnabled(true);
            serialUi->m_refreshBtn->setEnabled(false);
            emit logInfo(QString("[%1] 打开串口成功：%2").arg(QDateTime::currentDateTime().toString()).arg(portName));
        } else {
            emit logError(QString("[%1] 打开串口失败：%2").arg(QDateTime::currentDateTime().toString()).arg(m_serial->errorString()));
        }
    } else {
        // 关闭串口
        m_serial->close();
        m_isPortOpen = false;
        serialUi->m_openBtn->setText("打开串口");
        serialUi->m_sendBtn->setEnabled(false);
        serialUi->m_refreshBtn->setEnabled(true);
        emit logInfo(QString("[%1] 关闭串口成功").arg(QDateTime::currentDateTime().toString()));
    }
}

// 发送数据
void SerialModule::onSendDataClicked()
{
    QString sendStr = serialUi->m_sendEdit->text().trimmed();
    if (sendStr.isEmpty()) return;

    QByteArray sendData;
    if (serialUi->m_hexSendCheck->isChecked()) {
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
    if (serialUi->m_hexRecvCheck->isChecked()) {
        recvStr = byteArrayToHex(recvData);
        emit logInfo(QString("[%1] 接收（十六进制）：%2").arg(QDateTime::currentDateTime().toString()).arg(recvStr));
    } else {
        recvStr = QString::fromLocal8Bit(recvData);
        emit logInfo(QString("[%1] 接收：%2").arg(QDateTime::currentDateTime().toString()).arg(recvStr));
    }

    serialUi->m_recvEdit->append(recvStr);
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
            serialUi->m_openBtn->setText("打开串口");
            serialUi->m_sendBtn->setEnabled(false);
            serialUi->m_refreshBtn->setEnabled(true);
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