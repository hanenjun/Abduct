#include "WiFiManager.h"

// 构造函数
WiFiManager::WiFiManager() {}

// 初始化WiFi
void WiFiManager::initWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    startMillis = millis();
    isConnecting = true;
    Serial.println("Connecting to WiFi...");
}

// 扫描WiFi网络
void WiFiManager::scanNetworks() {
    int n = WiFi.scanNetworks();
    Serial.println("扫描到的WiFi网络:");
    if (n == 0) {
        Serial.println("没有找到WiFi网络");
    } else {
        for (int i = 0; i < n; ++i) {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(" dBm) ");
            Serial.print(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Open" : "Encrypted");
            Serial.println("");
            delay(10);
        }
    }
}

// 连接到WiFi
bool WiFiManager::connectWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    startMillis = millis(); // 重置时间计数
    isConnecting = true;
    Serial.println("Connecting to WiFi...");

    return true;  // 继续在loop中检查连接
}

// 断开WiFi连接
void WiFiManager::disconnectWiFi() {
    WiFi.disconnect();
    Serial.println("已断开WiFi连接");
}

// 获取WiFi状态
int WiFiManager::getWiFiStatus() {
    return WiFi.status();
}

// 获取当前WiFi的IP地址
String WiFiManager::getIPAddress() {
    return WiFi.localIP().toString();
}

// 获取当前连接的WiFi名称
String WiFiManager::getCurrentSSID() {
    return WiFi.SSID();
}
