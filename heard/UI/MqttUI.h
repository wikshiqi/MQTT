//// MqttUI.h
//#ifndef MQTTUI_H
//#define MQTTUI_H
//
//#include <QWidget>
//#include <QVBoxLayout>
//#include <QCheckBox>
//
//#include <QTextEdit.h>
//#include "GlobalDef.h"
//#include "QLineEdit.h"
//#include <QPushButton.h>
//// 纯UI层，无业务逻辑
//class MqttUI : public QWidget
//{
//Q_OBJECT
//public:
//    explicit MqttUI(QWidget *parent = nullptr);
//    ~MqttUI() override;
//    QLineEdit* m_usernameEdit;
//    QLineEdit* m_passwordEdit;
//    // 获取UI配置（供Core层使用）
//    MqttConfig getMqttConfig() const;
//    // 获取要发布的消息/主题
//    QString getPublishTopic() const;
//    QByteArray getPublishMsg() const;
//    // 获取要订阅的主题
//    QString getSubscribeTopic() const;
//    // 更新UI状态（接收Core层通知）
//    void updateMqttState(bool isConnected);
//    void appendReceivedMsg(const QString& topic, const QString& msg);
//    void showError(const QString& error);
//
//    QPushButton* m_brokerCard;     // Broker地址卡片
//    QComboBox* m_portCombo;            // 端口下拉框
//    QLineEdit* m_clientIdEdit;         // 客户端ID输入框
//    QCheckBox* m_autoReconnectCheck;   // 自动重连复选框
//
//    // 密码输入框
//    QPushButton* m_connectBtn;         // 连接按钮
//    QPushButton* m_disconnectBtn;      // 断开按钮
//    QLineEdit* m_publishTopicEdit;     // 发布主题输入框
//    QTextEdit* m_publishMsgEdit;       // 发布消息输入框
//    QPushButton* m_publishBtn;         // 发布按钮
//    QLineEdit* m_subscribeTopicEdit;   // 订阅主题输入框
//    QPushButton* m_subscribeBtn;       // 订阅按钮
//    QTextEdit* m_receiveMsgEdit;       // 接收消息显示框
//
//    // Qt原生布局
//    QVBoxLayout* m_mainLayout;
//       // 用户名输入框
//signals:
//    // UI事件信号（转发给Core层）
//    void connectMqttClicked();
//    void disconnectMqttClicked();
//    void publishMsgClicked();
//    void subscribeTopicClicked();
//
//private:
//    // 初始化UI
//    void initUI();
//    void initStyle();
//
//    // ElaTools组件
//
//};
//
//#endif // MQTTUI_H