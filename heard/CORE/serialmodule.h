#ifndef SERIALMODULE_H
#define SERIALMODULE_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QDebug>
class SerialModule : public QWidget
{
    Q_OBJECT
public:
    explicit SerialModule(QWidget *parent = nullptr);
    ~SerialModule() override;

signals:
    void logInfo(const QString& info);       // 日志信号
    void logError(const QString& error);     // 错误信号

private slots:
    // 界面操作
    void onRefreshPortClicked();             // 刷新串口列表
    void onOpenPortClicked();                // 打开/关闭串口
    void onSendDataClicked();                // 发送数据
    // 串口核心逻辑
    void onSerialReadyRead();                // 读取串口数据
    void onSerialErrorOccurred(QSerialPort::SerialPortError error); // 串口错误

private:
    // 界面控件
    QComboBox* m_portCombo;      // 串口选择
    QComboBox* m_baudRateCombo;  // 波特率
    QComboBox* m_dataBitsCombo;   // 数据位
    QComboBox* m_parityCombo;     // 校验位
    QComboBox* m_stopBitsCombo;   // 停止位
    QPushButton* m_refreshBtn;    // 刷新串口
    QPushButton* m_openBtn;       // 打开/关闭串口
    QTextEdit* m_recvEdit;        // 接收区
    QLineEdit* m_sendEdit;        // 发送区
    QPushButton* m_sendBtn;       // 发送按钮
    QCheckBox* m_hexSendCheck;    // 十六进制发送
    QCheckBox* m_hexRecvCheck;    // 十六进制接收

    // 串口核心变量
    QSerialPort* m_serial;        // 串口对象
    bool m_isPortOpen;            // 串口是否打开

    // 辅助函数
    QString byteArrayToHex(const QByteArray& data); // 字节数组转十六进制字符串
    QByteArray hexToByteArray(const QString& hex);   // 十六进制字符串转字节数组
};

#endif // SERIALMODULE_H