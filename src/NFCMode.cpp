#include "NFCMode.h"

// 构造函数，初始化 PN532 NFC 模块
NFC::NFC(int NFC_SDA_PIN,
         int NFC_SCL_PIN) : nfc(NFC_SDA_PIN,
                                NFC_SCL_PIN),
                            _NFC_SCL_PIN(NFC_SCL_PIN),
                            _NFC_SDA_PIN(NFC_SDA_PIN), NFCState(false), currentMode(NONE) {}

bool NFC::initNFC()
{
  delay(200);
  Serial.println("Initializing PN532...");
  Wire.begin(_NFC_SDA_PIN, _NFC_SCL_PIN); // 初始化 I2C 总线
  delay(200);                     // 等待 I2C 初始化
  if (!nfc.begin())
  {
    Serial.println("Couldn't find PN532, check connections!");
    return false;
  }
  nfc.SAMConfig(); // 配置 PN532 模块
  NFCState = true;
  Serial.println("PN532 Initialized. Ready for mode selection.");
  return true;
}

bool NFC::setMode(NFCMode mode)
{
  if(!mode) {
    mode = NFCMode::NONE;
  }
  if (!NFCState)
  {
    Serial.println("NFC module is not initialized.");
    return false;
  }
  currentMode = mode;
  switch (mode)
  {
  case READ:
    Serial.println("Set to READ mode.");
    break;
  case WRITE:
    Serial.println("Set to WRITE mode.");
    break;
  case NONE:
    Serial.println("Set to NONE mode.");
    break;
  }
  return true;
}

bool NFC::setReadMode()
{
  return setMode(READ);
}

bool NFC::setWriteMode()
{
  return setMode(WRITE);
}

bool NFC::setNoneMode()
{
  return setMode(NONE);
}

String NFC::readCard()
{
  if (!NFCState)
  {
    Serial.println("NFC module is not initialized.");
    return "NFC module is not initialized.";
  }
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;
  String uidString = "";

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success)
  {
    Serial.println("Found an NFC card!");
    uidString += "UID Length: " + String(uidLength) + " bytes\n";
    uidString += "UID Value: ";
    for (uint8_t i = 0; i < uidLength; i++)
    {
      uidString += " 0x" + String(uid[i], HEX);
    }
    uidString += "\n";
  }
  else
  {
    uidString = "No card detected, waiting...\n";
  }
  delay(200); // 等待 200 毫秒再检测
  return uidString;
}

bool NFC::authenticateWithKeyA(uint8_t *uid, uint8_t uidLength, uint8_t blockNumber, uint8_t *keyA)
{
  for (size_t i = 0; i < sizeof(COMMON_KEYS) / sizeof(COMMON_KEYS[0]); i++)
  {
    memcpy(keyA, COMMON_KEYS[i], 6);
    if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, 0, keyA))
    {
      Serial.print("Common KeyA found for block ");
      Serial.print(blockNumber);
      Serial.print(": ");
      for (int j = 0; j < 6; j++)
      {
        Serial.print(" 0x");
        Serial.print(keyA[j], HEX);
      }
      Serial.println();
      return true;
    }
  }

  // 暴力破解 KeyA
  Serial.println("Attempting brute force for KeyA...");
  for (uint32_t i = 0; i < 0xFFFFFFFF; i++)
  {
    for (int j = 0; j < 6; j++)
    {
      keyA[j] = (i >> (8 * j)) & 0xFF;
    }
    if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, blockNumber, 0, keyA))
    {
      Serial.print("KeyA found for block ");
      Serial.print(blockNumber);
      Serial.print(": ");
      for (int k = 0; k < 6; k++)
      {
        Serial.print(" 0x");
        Serial.print(keyA[k], HEX);
      }
      Serial.println();
      return true;
    }
  }

  return false;
}

bool NFC::writeCard(const uint8_t *data, size_t dataLength)
{
  if (!NFCState)
  {
    Serial.println("NFC module is not initialized.");
    return false;
  }
  uint8_t success;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success)
  {
    Serial.println("Found an NFC card to write data to!");

    uint8_t keyA[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // 默认的KeyA（用于认证）

    // 遍历卡片上的所有数据块
    for (uint8_t blockNumber = 0; blockNumber < 16; blockNumber++)
    {
      Serial.print("Trying to brute force KeyA for block ");
      Serial.println(blockNumber);

      if (!authenticateWithKeyA(uid, uidLength, blockNumber, keyA))
      {
        Serial.print("KeyA brute force failed for block ");
        Serial.println(blockNumber);
        continue;
      }

      uint8_t successWrite = nfc.mifareclassic_WriteDataBlock(blockNumber, const_cast<uint8_t *>(data));
      if (successWrite)
      {
        Serial.print("Data written to block ");
        Serial.println(blockNumber);
        return true;
      }
      else
      {
        Serial.print("Failed to write data to block ");
        Serial.println(blockNumber);
      }
    }
  }
  else
  {
    Serial.println("No card detected for writing, waiting...");
    return false;
  }
  return false;
}