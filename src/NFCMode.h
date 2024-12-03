#ifndef PN532_H
#define PN532_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

// 定义 SDA 和 SCL 引脚
constexpr int NFC_SDA_PIN = 20;  // 或 const int NFC_SDA_PIN = 20;
constexpr int NFC_SCL_PIN = 19;  // 或 const int NFC_SCL_PIN = 19;

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
  const uint8_t COMMON_KEYS[4][6] = {
      {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 默认出厂密钥
      {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5}, // 一种常见密钥
      {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7}, // 另一种常见密钥
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 全零密钥
  };
  int _NFC_SCL_PIN, _NFC_SDA_PIN;
  bool setMode(NFCMode mode);
  bool authenticateWithKeyA(uint8_t *uid, uint8_t uidLength, uint8_t blockNumber, uint8_t *keyA);

public:
  NFC(int NFC_SDA_PIN = NFC_SDA_PIN,
      int NFC_SCL_PIN = NFC_SCL_PIN);
  bool initNFC();
  bool setReadMode();
  bool setWriteMode();
  bool setNoneMode();
  NFCMode currentMode;
  String readCard();
  bool NFCState;
  bool writeCard(const uint8_t *data, size_t dataLength);
};

#endif
