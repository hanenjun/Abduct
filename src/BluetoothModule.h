#ifndef BLUETOOTH_MODULE_H
#define BLUETOOTH_MODULE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

class BluetoothModule {
public:
    BluetoothModule();  // 构造函数
    void initBLE(const char* deviceName);  // 初始化BLE并设置设备名称
    void startScanning(int scanTime);  // 开始扫描附近的蓝牙设备
    void stopScanning();  // 停止扫描
    void connectToDevice(BLEAdvertisedDevice advertisedDevice);  // 连接到扫描到的设备
    void disconnect();  // 断开连接
    void printScanResults();  // 打印扫描结果
    void printDeviceInfo();  // 打印设备信息

private:
    BLEScan* pBLEScan;  // BLE扫描对象
    BLEClient* pClient;  // BLE客户端对象
    String deviceName;  // 蓝牙设备名称
};

#endif // BLUETOOTH_MODULE_H
