#include "WiFiModule.h"

// 构造函数
WiFiModule::WiFiModule() {
    _ssid = "";
    _password = "";
}

// 初始化Wi-Fi连接
void WiFiModule::initWiFi(const char* ssid, const char* password) {
    _ssid = ssid;
    _password = password;

    WiFi.begin(_ssid, _password);  // 开始连接Wi-Fi
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {  // 等待连接成功
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected to WiFi");
    printIPAddress();
}

// 连接Wi-Fi
void WiFiModule::connectWiFi(const char* ssid, const char* password) {
    _ssid = ssid;
    _password = password;

    WiFi.begin(_ssid, _password);  // 开始连接Wi-Fi
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {  // 等待连接成功
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected to WiFi");
    printIPAddress();
}

// 断开Wi-Fi连接
void WiFiModule::disconnectWiFi() {
    WiFi.disconnect();
    Serial.println("Disconnected from WiFi");
}

// 打印Wi-Fi状态
void WiFiModule::printWiFiStatus() {
    switch (WiFi.status()) {
        case WL_CONNECTED:
            Serial.println("WiFi is connected");
            break;
        case WL_NO_SSID_AVAIL:
            Serial.println("WiFi SSID not available");
            break;
        case WL_CONNECT_FAILED:
            Serial.println("WiFi connection failed");
            break;
        default:
            Serial.println("WiFi is not connected");
            break;
    }
}

// 打印本机IP地址
void WiFiModule::printIPAddress() {
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

// 检查Wi-Fi连接状态
bool WiFiModule::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}


// #include <Arduino.h>
// #include "WiFiModule.h"

// WiFiModule wifi;  // 创建WiFiModule实例

// void setup() {
//     // 初始化串口
//     Serial.begin(115200);

//     // 配置Wi-Fi连接
//     const char* ssid = "your_ssid";
//     const char* password = "your_password";

//     // 使用WiFiModule类连接Wi-Fi
//     wifi.initWiFi(ssid, password);

//     // 检查Wi-Fi状态
//     wifi.printWiFiStatus();
// }

// void loop() {
//     // 主循环中，可以定期检查Wi-Fi状态
//     if (wifi.isConnected()) {
//         Serial.println("Wi-Fi is connected");
//     } else {
//         Serial.println("Wi-Fi is not connected");
//     }
//     delay(1000);
// }
