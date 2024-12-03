#ifndef CC1101MODULE_H
#define CC1101MODULE_H

#include <Arduino.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <SPI.h>

// 定义 CC1101 的引脚
int MISO_CPIN = 19;
int MOSI_CPIN = 13;
int SCK_CPIN = 14;
int CSN_CPIN = 5;
int GDO0_CPIN = 16; // 用于发送完成的信号
int GDO2_CPIN = 18;

class CC1101Module {
public:
    CC1101Module(int MISO_CPIN = MISO_CPIN, int MOSI_CPIN = MOSI_CPIN, int SCK_CPIN= SCK_CPIN, int CSN_CPIN= CSN_CPIN, int GDO0_CPIN= GDO0_CPIN, int GDO2_CPIN= GDO2_CPIN);
    void begin(bool transmit);
    void sendData(const char* message);
    void receiveData();
    bool isTransmissionComplete();
    
private:
    int _MISO_CPIN, _MOSI_CPIN, _SCK_CPIN, _CSN_CPIN, _GDO0_CPIN, _GDO2_CPIN;
    bool _transmit;
    uint8_t receivedData[64]; // 用于存储接收到的数据
};

#endif
