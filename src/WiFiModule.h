#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <WiFi.h>

class WiFiModule {
public:
    WiFiModule();  // 构造函数
    void initWiFi(const char* ssid, const char* password);  // 初始化Wi-Fi连接
    void connectWiFi(const char* ssid, const char* password);  // 连接Wi-Fi
    void disconnectWiFi();  // 断开Wi-Fi
    void printWiFiStatus();  // 打印Wi-Fi状态
    void printIPAddress();  // 打印本机IP地址
    bool isConnected();  // 检查Wi-Fi连接状态

private:
    const char* _ssid;  // Wi-Fi SSID
    const char* _password;  // Wi-Fi密码
};

#endif // WIFI_MODULE_H
