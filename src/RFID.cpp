#include <RFID.h>

RFID::RFID(int RFID_SS_PIN ,
           int RST_PIN  ,
           int RFID_MOSI_PIN  ,
           int RFID_MISO_PIN  ,
           int RFID_SCK_PIN  ) : mfrc522(RFID_SS_PIN, RST_PIN) {};
// 初始化 RFID 模块
void RFID::initRFID()
{
    SPI.begin(RFID_SCK_PIN, RFID_MISO_PIN, RFID_MOSI_PIN, RFID_SS_PIN); // 初始化 SPI 总线并设置引脚
    mfrc522.PCD_Init();                             // 初始化 MR522 模块
    Serial.println("Scan a card...");
}

// 打印卡片 UID
void RFID::printCardUID()
{
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX); // 打印卡片 UID
    }
    Serial.println();
}

// 写入数据到卡片
bool RFID::writeToCard(byte sector, byte block, const char *data)
{
    MFRC522::MIFARE_Key key;

    // 尝试常见密钥
    if (RFID::bruteForceWithCommonKeys(key))
    {
        // 认证成功，进行写操作
        if (RFID::writeDataToCard(sector, block, key, data))
        {
            Serial.println("Data successfully written.");
            return true;
        }
    }

    Serial.println("Failed to authenticate with common keys, starting brute force...");

    // 如果常见密钥失败，进行完整爆破
    if (RFID::bruteForceKey(key))
    {
        // 密钥爆破成功，进行写操作
        if (RFID::writeDataToCard(sector, block, key, data))
        {
            Serial.println("Data successfully written after brute force.");
            return true;
        }
    }

    Serial.println("Brute force failed!");
    return false; // 密钥爆破失败
}

// 使用常见密钥进行爆破
bool RFID::bruteForceWithCommonKeys(MFRC522::MIFARE_Key &key)
{
    for (int i = 0; i < sizeof(commonKeys) / sizeof(commonKeys[0]); i++)
    {
        for (byte j = 0; j < 6; j++)
        {
            key.keyByte[j] = commonKeys[i][j];
        }

        // 尝试认证
        if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 0, &key, &(mfrc522.uid)))
        {
            Serial.print("Authenticated with common key: ");
            printKey(key);
            return true;
        }
    }
    return false; // 常见密钥未成功
}

// 完整爆破密钥
bool RFID::bruteForceKey(MFRC522::MIFARE_Key &key)
{
    byte startKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // 从 0 开始
    byte endKey[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};   // 结束密钥

    // 从 startKey 到 endKey 进行尝试
    for (byte i = 0; i < 6; i++)
    {
        key.keyByte[i] = startKey[i];
    }

    do
    {
        if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 0, &key, &(mfrc522.uid)))
        {
            Serial.print("Authenticated with brute force key: ");
            printKey(key);
            return true; // 找到正确的密钥
        }

        // 递增密钥
        for (byte i = 5; i >= 0; i--)
        {
            if (key.keyByte[i] < 0xFF)
            {
                key.keyByte[i]++;
                break;
            }
            else
            {
                key.keyByte[i] = 0x00;
            }
        }
    } while (!isKeyEqual(key.keyByte, endKey)); // 如果没有达到结束密钥，则继续尝试

    return false; // 如果爆破失败
}

// 检查两个密钥是否相同
bool RFID::isKeyEqual(byte key1[6], byte key2[6])
{
    for (byte i = 0; i < 6; i++)
    {
        if (key1[i] != key2[i])
        {
            return false;
        }
    }
    return true;
}

// 打印密钥
void RFID::printKey(MFRC522::MIFARE_Key &key)
{
    for (byte i = 0; i < 6; i++)
    {
        Serial.print(key.keyByte[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

// 写数据到卡片
bool RFID::writeDataToCard(byte sector, byte block, MFRC522::MIFARE_Key &key, const char *data)
{
    // 认证扇区
    if (!mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sector * 4 + block, &key, &(mfrc522.uid)))
    {
        Serial.println("Authentication failed!");
        return false;
    }

    // 写入数据
    byte buffer[16];          // 存储需要写入的数据
    byte size = strlen(data); // 获取数据长度

    // 填充数据到缓冲区
    for (byte i = 0; i < size; i++)
    {
        buffer[i] = data[i];
    }

    // 填充剩余的空间
    for (byte i = size; i < 16; i++)
    {
        buffer[i] = 0x00; // 填充零
    }

    // 写入数据到卡片
    if (mfrc522.MIFARE_Write(sector * 4 + block, buffer, 16))
    {
        Serial.println("Data successfully written.");
        return true;
    }
    else
    {
        Serial.println("Write failed!");
        return false;
    }
}

// 读取卡片数据
bool RFID::readFromCard(byte sector, byte block)
{
    MFRC522::MIFARE_Key key;
    char outputData[16]; // Create a buffer to store the output data

    // 尝试使用常见的密钥进行认证
    if (RFID::bruteForceWithCommonKeys(key))
    {
        // 认证成功，进行读取操作
        Serial.println("Authentication successful with common key!");
        return RFID::readDataFromCard(sector, block, key, outputData);
    }

    Serial.println("Failed to authenticate with common keys, starting brute force...");

    // 如果常见密钥失败，进行完整爆破
    if (RFID::bruteForceKey(key))
    {
        // 密钥爆破成功，进行读取操作
        Serial.println("Authentication successful with brute-forced key!");
        return RFID::readDataFromCard(sector, block, key, outputData);
    }

    Serial.println("Brute force failed!");
    return false; // 密钥爆破失败
}

// 读取数据的辅助函数
String RFID::readDataFromCard(byte sector, byte block, MFRC522::MIFARE_Key &key, char *outputData)
{
    byte buffer[18]; // 用于存储读取的数据（MIFARE 卡的块大小为 16 字节）
    byte size = sizeof(buffer);

    // 认证扇区
    if (!mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, sector * 4 + block, &key, &(mfrc522.uid)))
    {
        Serial.println("Authentication failed!");
        return "Authentication failed!";
    }

    // 读取数据
    if (mfrc522.MIFARE_Read(sector * 4 + block, buffer, &size))
    {
        Serial.println("Data read from card:");

        // 将读取到的数据转换为字符串并打印
        for (byte i = 0; i < size; i++)
        {
            outputData[i] = (char)buffer[i];
            Serial.print(outputData[i]);
        }
        Serial.println();

        return String(outputData); // 返回读取到的数据
    }
    else
    {
        Serial.println("Read failed!");
        return "Read failed!";
    }
}

// RFID rfid; // 创建 RFID 对象，传入引脚配置

// void setup()
// {
//     Serial.begin(115200); // 初始化串口
//     rfid.initRFID();      // 初始化 RFID
// }

// void loop()
// {
//     if ( ! rfid.mfrc522.PICC_IsNewCardPresent() || ! rfid.mfrc522.PICC_ReadCardSerial())
//     {
//         return; // 没有检测到卡片，返回
//     }

//     rfid.printCardUID(); // 打印卡片 UID
//     delay(1000); // 延时以避免过多的扫描尝试
// }
