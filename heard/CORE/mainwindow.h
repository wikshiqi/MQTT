#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTextEdit>
#include "ChartUI.h"

class MqttUI;
class SerialUI;
class MqttModule;
class SerialModule;
class ChartUI;

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    // 修复：移除 = default，仅声明析构函数（让编译器自动生成默认实现）
    ~MainWindow() override;

public slots:
    void onLogInfo(const QString& info);
    void onLogError(const QString& error);

private:
    QTabWidget* m_tabWidget;
    MqttUI* m_mqttModule;
    SerialUI* m_serialModule;
    QTextEdit* m_globalLogEdit;
    ChartUI *m_chartUI;
    // 模块实例
    MqttModule* m_mqttModuleObj;
    SerialModule* m_serialModuleObj;
};

#endif // MAINWINDOW_H