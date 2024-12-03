#ifndef SDFILEUTILS_H
#define SDFILEUTILS_H

#include <SD.h>
#include <SPI.h>

#define SD_CS 7 // Chip select for SD card
class SDModule {
public:
    SDModule();
    void initSd();
    void listDir(File dir, const char *dirname, uint8_t levels);
    void createDir(File root, const char *path);
    void removeDir(File root, const char *path);
    void writeFile(File root, const char *path, const char *message);
    void appendFile(File root, const char *path, const char *message);
    void readFile(File root, const char *path);
    void deleteFile(File root, const char *path);
    void renameFile(File root, const char *oldName, const char *newName);
    void testFileIO(File root, const char *path);
};
#endif // SDFILEUTILS_H
