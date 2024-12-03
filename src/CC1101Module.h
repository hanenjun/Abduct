#ifndef CC1101MODULE_H
#define CC1101MODULE_H

#include <Arduino.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <SPI.h>

// 定义 CC1101 的引脚（可根据需要修改）
const int MISO_CPIN = 19;
const int MOSI_CPIN = 13;
const int SCK_CPIN = 14;
const int CSN_CPIN = 5;
const int GDO0_CPIN = 16;  // 用于发送完成的信号
const int GDO2_CPIN = 18;

class CC1101Module {
public:
    // 构造函数，传入SPI和GDO引脚，默认为定义的引脚
    CC1101Module(int MISO_CPIN = MISO_CPIN, int MOSI_CPIN = MOSI_CPIN, 
                 int SCK_CPIN = SCK_CPIN, int CSN_CPIN = CSN_CPIN, 
                 int GDO0_CPIN = GDO0_CPIN, int GDO2_CPIN = GDO2_CPIN);

    // 初始化函数，传入是否为发送模式
    void begin(bool transmit);

    // 发送数据函数
    void sendData(const char* message);

    // 接收数据函数
    void receiveData();

    // 检查数据是否发送完成
    bool isTransmissionComplete();

private:
    int _MISO_CPIN, _MOSI_CPIN, _SCK_CPIN, _CSN_CPIN, _GDO0_CPIN, _GDO2_CPIN;
    bool transmitMode;  // 用于标记当前是发送模式还是接收模式
    uint8_t receivedData[64];  // 用于存储接收到的数据

    // 时间控制变量
    unsigned long lastTransmitTime; // 上次发送时间
    unsigned long transmitDelay;    // 发送延时（毫秒）

    unsigned long lastReceiveTime;  // 上次接收时间
    unsigned long receiveDelay;     // 接收延时（毫秒）
};

#endif // CC1101MODULE_H
