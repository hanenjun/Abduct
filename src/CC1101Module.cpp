#include "CC1101Module.h"

// 构造函数，初始化引脚
CC1101Module::CC1101Module(int MISO_CPIN, int MOSI_CPIN, int SCK_CPIN, int CSN_CPIN, int GDO0_CPIN, int GDO2_CPIN) {
    _MISO_CPIN = MISO_CPIN;
    _MOSI_CPIN = MOSI_CPIN;
    _SCK_CPIN = SCK_CPIN;
    _CSN_CPIN = CSN_CPIN;
    _GDO0_CPIN = GDO0_CPIN;
    _GDO2_CPIN = GDO2_CPIN;
    _transmit = false; // 默认接收模式
}

void CC1101Module::begin(bool transmit) {
    _transmit = transmit; // 设置为发送或接收模式

    Serial.begin(115200); // 初始化串口通信，用于调试输出

    // 设置 SPI 引脚
    ELECHOUSE_cc1101.setSpiPin(_SCK_CPIN, _MISO_CPIN, _MOSI_CPIN, _CSN_CPIN);
    ELECHOUSE_cc1101.setGDO(_GDO0_CPIN, _GDO2_CPIN);
    delay(3000);

    if (ELECHOUSE_cc1101.getCC1101()) {
        Serial.println("Connection OK");
    } else {
        Serial.println("Connection Error");
    }

    // 配置发送或接收模式
    if (_transmit) {
        ELECHOUSE_cc1101.SetTx(); // 启用发送模式
        Serial.println("进入发送模式...");
    } else {
        ELECHOUSE_cc1101.setModulation(2);
        ELECHOUSE_cc1101.setMHZ(433.95);
        ELECHOUSE_cc1101.setPA(12);
        ELECHOUSE_cc1101.SetRx(); // 启用接收模式
        Serial.println("进入接收模式...");
    }
}

void CC1101Module::sendData(const char* message) {
    if (!_transmit) {
        Serial.println("设备未设置为发送模式!");
        return;
    }

    Serial.print("发送数据: ");
    Serial.println(message);

    // 启动发送
    ELECHOUSE_cc1101.SendData((uint8_t*)message, strlen(message));

    // 等待发送过程完成，增加适当的延时
    delay(1000); // 发送后的延时，确保数据有时间被发送

    // 检查 GDO0 引脚状态，确认发送是否完成
    int gdo0Status = digitalRead(_GDO0_CPIN);

    if (gdo0Status == HIGH) {
        Serial.println("数据发送成功!");
    } else {
        Serial.println("数据发送中...");
    }

    delay(1000); // 每秒发送一次数据
}

void CC1101Module::receiveData() {
    if (_transmit) {
        Serial.println("设备未设置为接收模式!");
        return;
    }

    // 如果设置为接收模式，接收数据
    if (ELECHOUSE_cc1101.CheckReceiveFlag()) { // 检查是否有接收数据
        int dataLength = ELECHOUSE_cc1101.ReceiveData(receivedData); // 接收数据
        Serial.print("接收到数据: CheckReceiveFlag");
        if (dataLength > 0) {
            Serial.print("接收到数据: ");
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
        // Serial.println("接收 FIFO 空");
    }
}

bool CC1101Module::isTransmissionComplete() {
    // 检查 GDO0 引脚状态，确认发送是否完成
    int gdo0Status = digitalRead(_GDO0_CPIN);
    return gdo0Status == HIGH;
}


// #include "CC1101Module.h"

// // 创建 CC1101 模块对象
// CC1101Module cc1101;

// void setup() {
//     // 初始化为接收模式
//     cc1101.begin(false);
// }

// void loop() {
//     // 如果是发送模式，发送数据
//     cc1101.sendData("HelloWorld");

//     // 如果是接收模式，接收数据
//     cc1101.receiveData();

//     // 每秒检查一次发送完成标志
//     if (cc1101.isTransmissionComplete()) {
//         Serial.println("数据发送完成");
//     }

//     delay(1000);
// }