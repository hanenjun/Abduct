#include "SDFileUtils.h"


SDModule::SDModule() {}

void SDModule::initSd() {
    // 初始化串口
    Serial.begin(115200);
    
    // 初始化SD卡，并指定Chip Select引脚
    if (!SD.begin(SD_CS)) {
        Serial.println("Card Mount Failed");
        return;
    }

    // 获取SD卡类型
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }

    // 输出SD卡类型
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    // 输出SD卡大小
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    // 打印SD卡的总空间和已用空间
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}


void SDModule::listDir(File dir, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);
    File entry = dir.openNextFile();
    while (entry) {
        if (entry.isDirectory()) {
            Serial.print("[DIR]  ");
            Serial.println(entry.name());
            if (levels) {
                listDir(entry, entry.name(), levels - 1);
            }
        } else {
            Serial.print("[FILE] ");
            Serial.print(entry.name());
            Serial.print("  SIZE: ");
            Serial.println(entry.size());
        }
        entry = dir.openNextFile();
    }
}

void SDModule::createDir(File root, const char *path) {
    if (SD.mkdir(path)) {
        Serial.print("Directory created: ");
        Serial.println(path);
    } else {
        Serial.print("Directory creation failed: ");
        Serial.println(path);
    }
}

void SDModule::removeDir(File root, const char *path) {
    if (SD.rmdir(path)) {
        Serial.print("Directory removed: ");
        Serial.println(path);
    } else {
        Serial.print("Directory removal failed: ");
        Serial.println(path);
    }
}

void SDModule::writeFile(File root, const char *path, const char *message) {
    File file = SD.open(path, FILE_WRITE);
    if (file) {
        file.print(message);
        file.close();
        Serial.print("Written to file: ");
        Serial.println(path);
    } else {
        Serial.print("Failed to open file for writing: ");
        Serial.println(path);
    }
}

void SDModule::appendFile(File root, const char *path, const char *message) {
    File file = SD.open(path, FILE_APPEND);
    if (file) {
        file.print(message);
        file.close();
        Serial.print("Appended to file: ");
        Serial.println(path);
    } else {
        Serial.print("Failed to open file for appending: ");
        Serial.println(path);
    }
}

void SDModule::readFile(File root, const char *path) {
    File file = SD.open(path);
    if (file) {
        Serial.print("Reading file: ");
        Serial.println(path);
        while (file.available()) {
            Serial.write(file.read());
        }
        file.close();
    } else {
        Serial.print("Failed to open file for reading: ");
        Serial.println(path);
    }
}

void SDModule::deleteFile(File root, const char *path) {
    if (SD.remove(path)) {
        Serial.print("File deleted: ");
        Serial.println(path);
    } else {
        Serial.print("Failed to delete file: ");
        Serial.println(path);
    }
}

void SDModule::renameFile(File root, const char *oldName, const char *newName) {
    if (SD.rename(oldName, newName)) {
        Serial.print("File renamed from ");
        Serial.print(oldName);
        Serial.print(" to ");
        Serial.println(newName);
    } else {
        Serial.print("Failed to rename file: ");
        Serial.println(oldName);
    }
}

void SDModule::testFileIO(File root, const char *path) {
    File file = SD.open(path, FILE_WRITE);
    if (file) {
        file.print("Testing file IO\n");
        file.close();
        Serial.println("File written successfully.");
    } else {
        Serial.println("Failed to write test file.");
    }
}
