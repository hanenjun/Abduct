#ifndef PN532_H
#define PN532_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

// 定义 SDA 和 SCL 引脚
constexpr int NFC_SDA_PIN = 20;
constexpr int NFC_SCL_PIN = 19;

// NFC 模块的工作模式
enum NFCMode
{
  NONE,
  READ,
  WRITE
};

class NFC
{
private:
  Adafruit_PN532 nfc;
  static const uint8_t COMMON_KEYS[4][6]; // 常见密钥数组
  int _NFC_SCL_PIN, _NFC_SDA_PIN;
  
  // 设置工作模式
  bool setMode(NFCMode mode);
  
  // 使用指定的 KeyA 进行身份验证
  bool authenticateWithKeyA(uint8_t *uid, uint8_t uidLength, uint8_t blockNumber, uint8_t *keyA);

  bool NFCState;         // NFC 模块初始化状态

  unsigned long lastNfcCheckTime; // Last time NFC was checked
  const unsigned long checkInterval; // Interval for non-blocking NFC checking
public:
  NFCMode currentMode;   // 当前工作模式
  // 构造函数
  NFC(int NFC_SDA_PIN = NFC_SDA_PIN, int NFC_SCL_PIN = NFC_SCL_PIN);

  // 初始化 NFC 模块
  bool initNFC();

  // 设置读卡模式
  bool setReadMode();

  // 设置写卡模式
  bool setWriteMode();

  // 设置无操作模式
  bool setNoneMode();

  // 读取卡片
  String readCard();

  // 写入数据到卡片
  bool writeCard(const uint8_t *data, size_t dataLength);
};

#endif
