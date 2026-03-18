// SerialUI.h
#ifndef SERIALUI_H
#define SERIALUI_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSerialPort>

//#include "ElaTextEdit.h"
#include "GlobalDef.h"

// 纯UI层，无业务逻辑
class SerialUI : public QWidget
{
Q_OBJECT
public:
    explicit SerialUI(QWidget *parent = nullptr);
    ~SerialUI() override;

    // 获取UI配置（供Core层使用）
    SerialConfig getSerialConfig() const;
    // 获取要发送的数据
    QByteArray getSendData() const;
    // 更新UI状态（接收Core层通知）
    void updateSerialState(bool isOpen);
    void appendReceivedData(const QString& data);
    void showError(const QString& error);
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

signals:
    // UI事件信号（转发给Core层）
    void openSerialClicked();
    void closeSerialClicked();
    void sendDataClicked();

private:
    // 初始化UI
    void initUI();
    void initStyle();

    // ElaTools组件

};

#endif // SERIALUI_H


