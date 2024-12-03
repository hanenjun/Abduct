#include "CC1101Module.h"

// 构造函数，初始化引脚
CC1101Module::CC1101Module(int MISO_CPIN, int MOSI_CPIN, int SCK_CPIN, int CSN_CPIN, int GDO0_CPIN, int GDO2_CPIN)
    : lastTransmitTime(0), transmitDelay(1000), lastReceiveTime(0), receiveDelay(100) {
    _MISO_CPIN = MISO_CPIN;
    _MOSI_CPIN = MOSI_CPIN;
    _SCK_CPIN = SCK_CPIN;
    _CSN_CPIN = CSN_CPIN;
    _GDO0_CPIN = GDO0_CPIN;
    _GDO2_CPIN = GDO2_CPIN;
    transmitMode = false; // 默认接收模式
}

void CC1101Module::begin(bool transmit) {
    transmitMode = transmit; // 设置为发送或接收模式

    Serial.begin(115200); // 初始化串口通信，用于调试输出

    // 设置 SPI 引脚
    ELECHOUSE_cc1101.setSpiPin(_SCK_CPIN, _MISO_CPIN, _MOSI_CPIN, _CSN_CPIN);
    ELECHOUSE_cc1101.setGDO(_GDO0_CPIN, _GDO2_CPIN);
    
    // 检查是否成功连接到CC1101模块
    if (ELECHOUSE_cc1101.getCC1101()) {
        Serial.println("Connection OK");
    } else {
        Serial.println("Connection Error");
    }

    // 配置发送或接收模式
    if (transmitMode) {
        ELECHOUSE_cc1101.SetTx(); // 启用发送模式
        Serial.println("进入发送模式...");
    } else {
        ELECHOUSE_cc1101.setModulation(2);  // 设置调制方式
        ELECHOUSE_cc1101.setMHZ(433.95);   // 设置频率
        ELECHOUSE_cc1101.setPA(12);        // 设置功率
        ELECHOUSE_cc1101.SetRx(); // 启用接收模式
        Serial.println("进入接收模式...");
    }
}

void CC1101Module::sendData(const char* message) {
    if (!transmitMode) {
        Serial.println("设备未设置为发送模式!");
        return;
    }

    // 检查是否到达发送间隔
    if (millis() - lastTransmitTime >= transmitDelay) {
        lastTransmitTime = millis(); // 更新上次发送时间

        Serial.print("发送数据: ");
        Serial.println(message);

        // 启动发送
        ELECHOUSE_cc1101.SendData((uint8_t*)message, strlen(message));

        // 检查 GDO0 引脚状态，确认发送是否完成
        if (isTransmissionComplete()) {
            Serial.println("数据发送成功!");
        } else {
            Serial.println("数据发送中...");
        }
    }
}

void CC1101Module::receiveData() {
    if (transmitMode) {
        Serial.println("设备未设置为接收模式!");
        return;
    }

    // 检查是否到达接收间隔
    if (millis() - lastReceiveTime >= receiveDelay) {
        lastReceiveTime = millis(); // 更新上次接收时间

        // 如果设置为接收模式，接收数据
        if (ELECHOUSE_cc1101.CheckReceiveFlag()) { // 检查是否有接收数据
            int dataLength = ELECHOUSE_cc1101.ReceiveData(receivedData); // 接收数据
            Serial.print("接收到数据: ");
            if (dataLength > 0) {
                for (int i = 0; i < dataLength; i++) {
                    Serial.print((char)receivedData[i]);
                }
                Serial.println();

                // 检查 CRC 是否有效
                if (ELECHOUSE_cc1101.CheckCRC()) {
                    Serial.println("CRC 校验成功！");
                } else {
                    Serial.println("CRC 校验失败！");
                }
            } else {
                Serial.println("无数据");
            }
        } else {
            // 如果没有接收到数据，可以考虑增加延时再进行下一次检查
            // Serial.println("接收 FIFO 空");
        }
    }
}

bool CC1101Module::isTransmissionComplete() {
    // 检查 GDO0 引脚状态，确认发送是否完成
    int gdo0Status = digitalRead(_GDO0_CPIN);
    return gdo0Status == HIGH;
}
