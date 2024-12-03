#ifndef RFID_H
#define RFID_H

#include <MFRC522.h>
#include <SPI.h>

// 引脚定义
#define RFID_SS_PIN 5    // 片选引脚（SDA）
#define RST_PIN 17       // 复位引脚
#define RFID_MOSI_PIN 13 // MOSI 引脚
#define RFID_MISO_PIN 12 // MISO 引脚
#define RFID_SCK_PIN 14  // SCK 引脚

class RFID {
public:
    MFRC522 mfrc522; // MFRC522 实例

    // 常见密钥列表（不允许修改）
    static const byte commonKeys[5][6]; 

    // 构造函数，初始化 MFRC522
    RFID(
        int RFID_SS_PIN = ::RFID_SS_PIN,
        int RST_PIN = ::RST_PIN,
        int RFID_MOSI_PIN = ::RFID_MOSI_PIN,
        int RFID_MISO_PIN = ::RFID_MISO_PIN,
        int RFID_SCK_PIN = ::RFID_SCK_PIN
    );

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
    // 扫描并读取所有卡片数据
    void scanAndReadAllCards();

    // 写数据到卡片
    bool writeDataToCard(byte sector, byte block, MFRC522::MIFARE_Key &key, const char *data);

    // 读取卡片数据
    bool readFromCard(byte sector, byte block);

    // 读取数据的辅助函数
    String readDataFromCard(byte sector, byte block, MFRC522::MIFARE_Key &key, char *outputData);

private:
    // 读取卡片 UID 是否匹配
    bool isUIDMatch(MFRC522::Uid &uid);

    // 增加密钥（递增）
    bool incrementKey(MFRC522::MIFARE_Key &key);
};

#endif // RFID_H
