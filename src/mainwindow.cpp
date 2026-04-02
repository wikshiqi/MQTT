#include "mainwindow.h"
#include "MqttUI.h"
#include "SerialUI.h"
#include "ChartUI.h"
#include "mqttmodule.h"
#include "serialmodule.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDateTime>
#include <QFont>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("MQTT");
    setMinimumSize(1000,700);

    QWidget* c = new QWidget(this);
    setCentralWidget(c);
    QVBoxLayout* l = new QVBoxLayout(c);
    l->setSpacing(10);
    l->setContentsMargins(10,10,10,10);

    m_tabWidget = new QTabWidget;
    m_mqttModule = new MqttUI;
    m_serialModule = new SerialUI;
    m_chartUI = new ChartUI;

    m_tabWidget->addTab(m_mqttModule, "MQTT");
    m_tabWidget->addTab(m_serialModule, "串口调试助手");
    m_tabWidget->addTab(m_chartUI, "📊 数据图表");

    l->addWidget(m_tabWidget);

    QGroupBox* g = new QGroupBox("全局日志");
    QVBoxLayout* lg = new QVBoxLayout(g);
    m_globalLogEdit = new QTextEdit;
    m_globalLogEdit->setReadOnly(true);
    m_globalLogEdit->setFont(QFont("Consolas",9));
    lg->addWidget(m_globalLogEdit);
    l->addWidget(g,1);

    m_mqttModuleObj = new MqttModule(m_mqttModule);
    m_serialModuleObj = new SerialModule(m_serialModule);



    connect(m_mqttModuleObj, &MqttModule::logInfo, this, &MainWindow::onLogInfo);
    connect(m_mqttModuleObj, &MqttModule::logError, this, &MainWindow::onLogError);
    connect(m_mqttModuleObj, &MqttModule::sendDataToChart, m_chartUI, &ChartUI::updateOneNetData);
    connect(m_serialModuleObj, &SerialModule::logInfo, this, &MainWindow::onLogInfo);
    connect(m_serialModuleObj, &SerialModule::logError, this, &MainWindow::onLogError);
}

MainWindow::~MainWindow() = default;

void MainWindow::onLogInfo(const QString& s){
    m_globalLogEdit->append("[INFO] " + s);
}
void MainWindow::onLogError(const QString& s){
    m_globalLogEdit->append("[ERROR] " + s);
}