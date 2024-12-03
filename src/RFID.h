#ifndef RFID_H
#define RFID_H

#include <MFRC522.h>
#include <SPI.h>

// 引脚定义
int RFID_SS_PIN = 5;    // 片选引脚（SDA）
int RST_PIN = 17;       // 复位引脚
int RFID_MOSI_PIN = 13; // MOSI 引脚
int RFID_MISO_PIN = 12; // MISO 引脚
int RFID_SCK_PIN = 14;  // SCK 引脚

class RFID
{
public:
    MFRC522 mfrc522; // MFRC522 实例
    // 常见密钥列表
    const byte commonKeys[5][6] = {
        {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5}, // 密钥 A
        {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5}, // 密钥 B
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 默认密钥 FF
        {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB}, // 常见密钥 1
        {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}, // 常见密钥 2
    };
    // 常见密钥列表（固定大小的数组）
    // static const byte commonKeys[5][6];

    // 构造函数，初始化 MFRC522
    RFID(
        int RFID_SS_PIN = RFID_SS_PIN,
        int RST_PIN = RST_PIN,
        int RFID_MOSI_PIN = RFID_MOSI_PIN,
        int RFID_MISO_PIN = RFID_MISO_PIN,
        int RFID_SCK_PIN = RFID_SCK_PIN);

    // 初始化 RFID 模块
    void initRFID();

    // 打印卡片 UID
    void printCardUID();

    // 写入数据到卡片
    bool writeToCard(byte sector, byte block, const char *data);

    // 使用常见密钥进行爆破
    bool bruteForceWithCommonKeys(MFRC522::MIFARE_Key &key);

    // 完整爆破密钥
    bool bruteForceKey(MFRC522::MIFARE_Key &key);

    // 检查两个密钥是否相同
    bool isKeyEqual(byte key1[6], byte key2[6]);

    // 打印密钥
    void printKey(MFRC522::MIFARE_Key &key);

    // 写数据到卡片
    bool writeDataToCard(byte sector, byte block, MFRC522::MIFARE_Key &key, const char *data);

    // 读取卡片数据
    bool readFromCard(byte sector, byte block);

    // 读取数据的辅助函数
    String readDataFromCard(byte sector, byte block, MFRC522::MIFARE_Key &key, char *outputData);

private:
    // 读取卡片 UID 是否匹配
    bool isUIDMatch(MFRC522::Uid &uid);
};

#endif // RFID_H
