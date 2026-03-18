#include "mainwindow.h"
#include "UI/MqttUI.h"
#include "UI/SerialUI.h"
#include "mqttmodule.h"
#include "serialmodule.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDateTime>
#include <QFont>
#include <QTabWidget>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // ========== 主窗口设置 ==========
    setWindowTitle("MQTT+串口调试助手（手搓协议）");
    setMinimumSize(1000, 700);

    // ========== 中心部件 ==========
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 1. 标签页（MQTT + 串口）
    m_tabWidget = new QTabWidget(this);
    m_mqttModule = new MqttUI(this);
    m_serialModule = new SerialUI(this);
    m_tabWidget->addTab(m_mqttModule, "MQTT（OneNET）");
    m_tabWidget->addTab(m_serialModule, "串口调试助手");
    mainLayout->addWidget(m_tabWidget);

    // 2. 全局日志框
    QGroupBox* logGroup = new QGroupBox("全局日志", this);
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);
    m_globalLogEdit = new QTextEdit(this);
    m_globalLogEdit->setReadOnly(true);
    m_globalLogEdit->setFont(QFont("Consolas", 9));
    logLayout->addWidget(m_globalLogEdit);
    mainLayout->addWidget(logGroup, 1); // 占1份高度

    // ========== 初始化模块并绑定信号槽 ==========
    m_mqttModuleObj = new MqttModule(m_mqttModule, this);
    m_serialModuleObj = new SerialModule(m_serialModule, this);

    // MQTT模块日志
    connect(m_mqttModuleObj, &MqttModule::logInfo, this, &MainWindow::onLogInfo);
    connect(m_mqttModuleObj, &MqttModule::logError, this, &MainWindow::onLogError);
    // 串口模块日志
    connect(m_serialModuleObj, &SerialModule::logInfo, this, &MainWindow::onLogInfo);
    connect(m_serialModuleObj, &SerialModule::logError, this, &MainWindow::onLogError);
}

MainWindow::~MainWindow() = default;

// 全局日志-信息
void MainWindow::onLogInfo(const QString& info)
{
    m_globalLogEdit->append(QString("[INFO] %1").arg(info));
    m_globalLogEdit->setTextColor(Qt::black);
}

// 全局日志-错误
void MainWindow::onLogError(const QString& error)
{
    m_globalLogEdit->append(QString("[ERROR] %1").arg(error));
    m_globalLogEdit->setTextColor(Qt::red);
}