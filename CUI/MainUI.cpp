// MainUI.cpp
#include "UI/MainUI.h"
#include <QVBoxLayout>
#include <QFont>

MainUI::MainUI(QWidget *parent) : QWidget(parent)
{
    initUI();
}

MainUI::~MainUI() = default;

void MainUI::initUI()
{
    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. 顶部应用栏（ElaTools）
    m_appBar = new QTextEdit(this);
    m_appBar->setTitle("串口+MQTT调试工具");

    m_appBar->setSubTitle("基于Qt + ElaTools");
    mainLayout->addWidget(m_appBar);

    // 2. 标签页（串口/MQTT）
    m_tabWidget = new QTabWidget(this);
    m_serialUI = new SerialUI(this);
    m_mqttUI = new MqttUI(this);

    m_tabWidget->addTab(m_serialUI, "串口通信");
    m_tabWidget->addTab(m_mqttUI, "MQTT通信");
    mainLayout->addWidget(m_tabWidget);

    // 窗口样式
    setWindowTitle("串口+MQTT调试工具");
    resize(800, 600);
    QFont font("Microsoft YaHei", 9);
    setFont(font);
}