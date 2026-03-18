#ifndef GLOBALDEF_H
#define GLOBALDEF_H

#include <QString>

// 串口配置结构体
struct SerialConfig {
    QString portName;   // 串口名（COM1/COM2）
    int baudRate;       // 波特率（9600/115200）
    int dataBits;       // 数据位
    int stopBits;       // 停止位
    int parity;         // 校验位
};

// MQTT配置结构体
struct MqttConfig {
    QString broker;     // 服务器地址
    int port;           // 端口
    QString clientId;   // 客户端ID
    bool autoReconnect; // 自动重连
    QString username;   // 用户名（可选）
    QString password;   // 密码（可选）
};

#endif // GLOBALDEF_H