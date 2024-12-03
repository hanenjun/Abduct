#include <MFRC522.h>
#include <SPI.h>

// 常见密钥数组
const byte commonKeys[4][6] = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, // 默认出厂密钥
    {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5}, // 常见密钥1
    {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7}, // 常见密钥2
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 全零密钥
};

class RFID
{
private:
    MFRC522 mfrc522;
    byte RFID_SCK_PIN, RFID_MISO_PIN, RFID_MOSI_PIN, RFID_SS_PIN, RST_PIN;

public:
    RFID(int RFID_SS_PIN, int RST_PIN, int RFID_MOSI_PIN, int RFID_MISO_PIN, int RFID_SCK_PIN)
        : mfrc522(RFID_SS_PIN, RST_PIN)
    {
        this->RFID_SS_PIN = RFID_SS_PIN;
        this->RST_PIN = RST_PIN;
        this->RFID_MOSI_PIN = RFID_MOSI_PIN;
        this->RFID_MISO_PIN = RFID_MISO_PIN;
        this->RFID_SCK_PIN = RFID_SCK_PIN;
    }

    // 初始化 RFID 模块
    void initRFID()
    {
        SPI.begin(RFID_SCK_PIN, RFID_MISO_PIN, RFID_MOSI_PIN, RFID_SS_PIN); // 初始化 SPI 总线
        mfrc522.PCD_Init();                                                 // 初始化 MFRC522 模块
        Serial.println("Scan a card...");
    }

    // 打印卡片 UID
    void printCardUID()
    {
        for (byte i = 0; i < mfrc522.uid.size; i++)
        {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX); // 打印卡片 UID
        }
        Serial.println();
    }
    bool readFromCard(byte sector, byte block)
    {
        MFRC522::MIFARE_Key key;

        // 尝试使用常见密钥进行认证
        if (bruteForceWithCommonKeys(key)) // 使用常见密钥进行认证
        {
            Serial.println("Authentication successful with common key!");

            // 读取数据
            String data = readDataFromCard(sector, block, key); // 调用现有的读取数据方法

            // 打印读取的数据
            Serial.print("Data read from sector ");
            Serial.print(sector);
            Serial.print(", block ");
            Serial.print(block);
            Serial.print(": ");
            Serial.println(data);

            // 如果成功读取数据，返回 true
            if (data != "Authentication failed!" && data != "Failed to read data!")
            {
                return true;
            }
        }
        else
        {
            Serial.println("Authentication failed with common keys, starting brute force...");

            // 如果常见密钥失败，进行完整爆破
            if (bruteForceKey(key)) // 使用暴力破解方式认证
            {
                Serial.println("Authentication successful with brute-forced key!");

                // 读取数据
                String data = readDataFromCard(sector, block, key); // 调用现有的读取数据方法

                // 打印读取的数据
                Serial.print("Data read from sector ");
                Serial.print(sector);
                Serial.print(", block ");
                Serial.print(block);
                Serial.print(": ");
                Serial.println(data);

                // 如果成功读取数据，返回 true
                if (data != "Authentication failed!" && data != "Failed to read data!")
                {
                    return true;
                }
            }
            else
            {
                Serial.println("Brute force failed!");
            }
        }

        // 如果认证或读取失败，返回 false
        return false;
    }
    // 写入数据到卡片
    bool writeToCard(byte sector, byte block, const char *data)
    {
        MFRC522::MIFARE_Key key;

        // 尝试常见密钥
        if (bruteForceWithCommonKeys(key))
        {
            if (writeDataToCard(sector, block, key, data))
            {
                Serial.println("Data successfully written.");
                return true;
            }
        }

        Serial.println("Failed to authenticate with common keys, starting brute force...");

        // 如果常见密钥失败，进行完整爆破
        if (bruteForceKey(key))
        {
            if (writeDataToCard(sector, block, key, data))
            {
                Serial.println("Data successfully written after brute force.");
                return true;
            }
        }

        Serial.println("Brute force failed!");
        return false; // 密钥爆破失败
    }

    // 使用常见密钥进行爆破
    bool bruteForceWithCommonKeys(MFRC522::MIFARE_Key &key)
    {
        for (int i = 0; i < sizeof(commonKeys) / sizeof(commonKeys[0]); i++)
        {
            memcpy(key.keyByte, commonKeys[i], sizeof(key.keyByte));

            if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 0, &key, &(mfrc522.uid)))
            {
                Serial.print("Authenticated with common key: ");
                printKey(key);
                return true;
            }
        }
        return false; // 常见密钥未成功
    }

    // 写数据到卡片
    bool writeDataToCard(byte sector, byte block, MFRC522::MIFARE_Key &key, const char *data)
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
    // 完整爆破密钥
    bool bruteForceKey(MFRC522::MIFARE_Key &key)
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
        } while (incrementKey(key)); // 如果没有达到结束密钥，则继续尝试

        return false; // 如果爆破失败
    }

    // 增加密钥，检查是否达到最大值
    bool incrementKey(MFRC522::MIFARE_Key &key)
    {
        for (byte i = 5; i >= 0; i--)
        {
            if (key.keyByte[i] < 0xFF)
            {
                key.keyByte[i]++;
                return true;
            }
            else
            {
                key.keyByte[i] = 0x00;
            }
        }
        return false;
    }

    // 打印密钥
    void printKey(MFRC522::MIFARE_Key &key)
    {
        for (byte i = 0; i < 6; i++)
        {
            Serial.print(key.keyByte[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    // 扫描并读取所有卡片数据
    void scanAndReadAllCards()
    {
        // 检查是否有卡片存在
        if (mfrc522.PICC_IsNewCardPresent())
        {
            // 选择卡片
            if (mfrc522.PICC_ReadCardSerial())
            {
                // 打印卡片 UID
                Serial.print("Card UID: ");
                printCardUID();

                MFRC522::MIFARE_Key key;

                // 尝试使用常见密钥进行认证
                if (bruteForceWithCommonKeys(key))
                {
                    Serial.println("Authentication successful with common key!");
                    // 扫描所有扇区（默认最大扇区数为 16）
                    for (byte sector = 0; sector < 16; sector++)
                    {
                        for (byte block = 0; block < 4; block++)
                        {
                            Serial.print("Reading sector ");
                            Serial.print(sector);
                            Serial.print(", block ");
                            Serial.print(block);
                            String data = readDataFromCard(sector, block, key);
                            Serial.print("Data: ");
                            Serial.println(data);
                        }
                    }
                }
                else
                {
                    Serial.println("Authentication failed with common keys, starting brute force...");
                    // 如果常见密钥失败，进行完整爆破
                    if (bruteForceKey(key))
                    {
                        Serial.println("Authentication successful with brute-forced key!");
                        // 扫描所有扇区（默认最大扇区数为 16）
                        for (byte sector = 0; sector < 16; sector++)
                        {
                            for (byte block = 0; block < 4; block++)
                            {
                                Serial.print("Reading sector ");
                                Serial.print(sector);
                                Serial.print(", block ");
                                Serial.print(block);
                                String data = readDataFromCard(sector, block, key);
                                Serial.print("Data: ");
                                Serial.println(data);
                            }
                        }
                    }
                    else
                    {
                        Serial.println("Brute force failed!");
                    }
                }
            }
        }
    }

    // 读取数据的辅助函数
    String readDataFromCard(byte sector, byte block, MFRC522::MIFARE_Key &key)
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
            Serial.println("Data read successfully!");
            String data = "";
            for (byte i = 0; i < size; i++)
            {
                data += (char)buffer[i];
            }
            return data;
        }
        else
        {
            Serial.println("Failed to read data!");
            return "Failed to read data!";
        }
    }
};
