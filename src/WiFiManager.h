#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>

class WiFiManager {
public:
    // 构造函数
    WiFiManager();

    // 初始化WiFi，连接到指定的网络
    void initWiFi(const char* ssid, const char* password);

    // 扫描WiFi AP列表
    void scanNetworks();

    // 连接WiFi
    bool connectWiFi(const char* ssid, const char* password);

    // 断开WiFi连接
    void disconnectWiFi();

    // 获取当前WiFi连接状态
    int getWiFiStatus();

    // 获取WiFi的IP地址
    String getIPAddress();

    // 获取已连接的WiFi名称
    String getCurrentSSID();

private:
    // WiFi连接超时（毫秒）
    unsigned startMillis;
    bool isConnecting;
    const unsigned long connectionTimeout = 30000;
};

#endif // WIFIMANAGER_H
