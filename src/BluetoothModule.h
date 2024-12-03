#ifndef BLUETOOTH_MODULE_H
#define BLUETOOTH_MODULE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// 蓝牙模块类
class BluetoothModule {
public:
    // 构造函数，初始化成员变量
    BluetoothModule();

    // 初始化BLE并设置设备名称
    void initBLE(const char* deviceName);

    // 开始扫描附近的蓝牙设备
    // scanTime：扫描持续的时间（秒）
    void startScanning(int scanTime);

    // 停止扫描
    void stopScanning();

    // 连接到扫描到的设备
    // advertisedDevice：扫描到的设备
    void connectToDevice(BLEAdvertisedDevice advertisedDevice);

    // 断开当前连接的设备
    void disconnect();

    // 打印扫描到的设备信息
    void printScanResults();

    // 打印当前连接设备的信息
    void printDeviceInfo();
private:
    // BLE扫描对象
    BLEScan* pBLEScan;

    // BLE客户端对象
    BLEClient* pClient;

    // 蓝牙设备名称
    String deviceName;
};

#endif // BLUETOOTH_MODULE_H
