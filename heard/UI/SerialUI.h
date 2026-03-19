#ifndef SERIALUI_H
#define SERIALUI_H

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QSerialPort>

// 定义串口配置结构体
struct SerialConfig {
    QString portName;
    int baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity parity;
};

class QComboBox;
class QPushButton;
class QLineEdit;
class QTextEdit;
class QCheckBox;

class SerialUI : public QWidget
{
Q_OBJECT // 必须添加Q_OBJECT宏

public:
    explicit SerialUI(QWidget *parent = nullptr);
    SerialConfig getSerialConfig() const;
    QByteArray getSendData() const;
    void updateSerialState(bool isOpen);
    void appendReceivedData(const QString &data);
    void showError(const QString &error);
    // 界面控件
    QComboBox* m_portCombo;
    QComboBox* m_baudRateCombo;
    QComboBox* m_dataBitsCombo;
    QComboBox* m_parityCombo;
    QComboBox* m_stopBitsCombo;
    QPushButton* m_refreshBtn;
    QPushButton* m_openBtn;
    QPushButton* m_sendBtn;
    QLineEdit* m_sendEdit;
    QTextEdit* m_recvEdit;
    QCheckBox* m_hexSendCheck;
    QCheckBox* m_hexRecvCheck;
signals:
    // 声明缺失的信号
    void openSerialPort();
    void closeSerialPort();
    void sendSerialData(const QByteArray& data);

public slots:
    void openSerialClicked(void);
    void closeSerialClicked(void);
    void sendDataClicked(void);
    void onRefreshPortClicked();

private:
    void initUI();
    void initStyle();


};

#endif // SERIALUI_H