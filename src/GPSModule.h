#ifndef GPS_TOOL_H
#define GPS_TOOL_H

#include <Arduino.h>
#include <SoftwareSerial.h>

// 默认引脚定义
constexpr int DEFAULT_RX_PIN = 16;
constexpr int DEFAULT_TX_PIN = 17;
constexpr int DEFAULT_PPS_PIN = 19;

class GPSModule
{
public:
    // 构造函数
    GPSModule(int RX_PIN = DEFAULT_RX_PIN, int TX_PIN = DEFAULT_TX_PIN, int PPS_PIN = DEFAULT_PPS_PIN);

    // GPS 初始化
    void initGPS(long baudRate = 9600);

    // 更新数据
    void update();

    // 解析NMEA数据
    void parseNMEAData(String data);

    // 解析不同类型的NMEA语句
    void parseGGA(String ggaData);
    void parseGSV(String gsvData);
    void parseGSA(String gsaData);
    void parseRMC(String rmcData);
    String getGPSTimeInfo();
    static void ppsISR();
    String gpsTime;
    bool isState = false;
private:
    SoftwareSerial mySerial;           // 创建软件串口对象
    String nmeaData;                   // 存储NMEA数据
    int _PPS_PIN;                      // PPS引脚（如果有用到）
    unsigned long lastUpdateTime;    // Last time NFC was checked
    const unsigned long initTimeout; // Interval for non-blocking NFC checking
};

#endif // GPS_TOOL_H
