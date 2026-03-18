#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include "mqttmodule.h"
#include "serialmodule.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // 统一日志显示
    void onLogInfo(const QString& info);
    void onLogError(const QString& error);

private:
    QTabWidget* m_tabWidget;    // 标签页
    MqttModule* m_mqttModule;   // MQTT模块
    SerialModule* m_serialModule; // 串口模块
    QTextEdit* m_globalLogEdit; // 全局日志框
};

#endif // MAINWINDOW_H