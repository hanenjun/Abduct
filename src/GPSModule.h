#ifndef GPS_TOOL_H
#define GPS_TOOL_H

#include <Arduino.h>
#include <SoftwareSerial.h>


// 定义 SDA 和 SCL 引脚
constexpr int RX_PIN = 16; 
constexpr int TX_PIN = 17; 
constexpr int PPS_PIN = 19; 

class GPSModule
{
public:
    GPSModule(int RX_PIN = RX_PIN, int TX_PIN = TX_PIN, int PPS_PIN = PPS_PIN);
    void initGPS(long baudRate = 9600, int RX_PIN = TX_PIN, int TX_PIN = PPS_PIN);
    void update();
    void parseNMEAData(String data);
    void parseGGA(String ggaData);
    void parseGSV(String gsvData);
    void parseGSA(String gsaData);

private:
    SoftwareSerial mySerial;
    String nmeaData;
    int _PPS_PIN;
};

#endif // GPS_TOOL_H
