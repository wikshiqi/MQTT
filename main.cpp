#include <QCoreApplication>
#include "mqttclient.h"
#include <QTimer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MqttClient* client = new MqttClient(&a); // 改为指针，避免栈变量捕获问题

    // 连接本地Mosquitto（如果改了端口，这里同步改）
    client->connectToBroker("127.0.0.1", 1883, "QtHandmadeMQTT_001");

    // 延迟订阅（明确捕获client指针）
    QTimer::singleShot(2000, [client]() {
        client->subscribe("test/qt", 0);
    });

    // 延迟发布
    QTimer::singleShot(3000, [client]() {
        client->publish("test/qt", "Hello Qt MQTT!", 0, false);
    });

    // 延迟断开连接并退出
    QTimer::singleShot(10000, [&a, client]() {
        client->disconnect();
        a.quit();
    });

    return a.exec();
}