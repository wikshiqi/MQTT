// MainUI.h
#ifndef MAINUI_H
#define MAINUI_H

#include <QWidget>
#include <QTabWidget>
#include "SerialUI.h"
#include "MqttUI.h"


// 主界面：整合串口和MQTT子界面
class MainUI : public QWidget
{
Q_OBJECT
public:
    explicit MainUI(QWidget *parent = nullptr);
    ~MainUI() override;

    // 暴露子UI对象（供外层绑定Core）
    SerialUI* getSerialUI() const { return m_serialUI; }
    MqttUI* getMqttUI() const { return m_mqttUI; }

private:
    void initUI();

    // ElaTools组件
    QTextEdit* m_appBar;         // 顶部应用栏
    QTabWidget* m_tabWidget;     // 标签页（串口/MQTT）
    SerialUI* m_serialUI;        // 串口子界面
    MqttUI* m_mqttUI;            // MQTT子界面
};

#endif // MAINUI_H

