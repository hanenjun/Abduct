#include "BluetoothModule.h"

// 自定义一个类继承 BLEAdvertisedDeviceCallbacks 并实现 onResult 方法
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        Serial.print("Advertised Device found: ");
        Serial.println(advertisedDevice.toString().c_str());
    }
};

// 在构造函数中使用该类的实例
BluetoothModule::BluetoothModule() {
    pClient = nullptr;
    pBLEScan = BLEDevice::getScan();  // 创建BLE扫描对象
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());  // 使用自定义的回调类
    pBLEScan->setActiveScan(true);  // 设置为主动扫描
}

// 初始化BLE并设置设备名称
void BluetoothModule::initBLE(const char* deviceName) {
    BLEDevice::init(deviceName);  // 初始化BLE设备
    this->deviceName = String(deviceName);
    Serial.print("BLE Device initialized with name: ");
    Serial.println(deviceName);
}

// 开始扫描附近的蓝牙设备
void BluetoothModule::startScanning(int scanTime) {
    Serial.println("Scanning for BLE devices...");
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Scan completed. Found ");
    Serial.print(foundDevices.getCount());
    Serial.println(" devices.");
    printScanResults();  // 打印扫描结果
}

// 停止扫描
void BluetoothModule::stopScanning() {
    pBLEScan->stop();
    Serial.println("Scanning stopped.");
}

// 连接到扫描到的设备
void BluetoothModule::connectToDevice(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("Connecting to device: ");
    Serial.println(advertisedDevice.getName().c_str());
    
    pClient = BLEDevice::createClient();  // 创建一个BLE客户端对象
    if (!pClient) {
        Serial.println("Failed to create BLE client.");
        return;
    }
    
    if (pClient->connect(&advertisedDevice)) {  // 使用指针传递
        Serial.println("Connected to device.");
        printDeviceInfo();  // 打印设备信息
    } else {
        Serial.println("Failed to connect to device.");
    }
}

// 断开连接
void BluetoothModule::disconnect() {
    if (pClient && pClient->isConnected()) {
        pClient->disconnect();
        Serial.println("Disconnected from device.");
    } else {
        Serial.println("No device is connected.");
    }
}

// 打印扫描结果
void BluetoothModule::printScanResults() {
    int deviceCount = pBLEScan->getResults().getCount();
    for (int i = 0; i < deviceCount; i++) {
        BLEAdvertisedDevice advertisedDevice = pBLEScan->getResults().getDevice(i);
        Serial.print("Device ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(advertisedDevice.getName().c_str());
        Serial.print(" | RSSI: ");
        Serial.println(advertisedDevice.getRSSI());
    }
}

// 打印设备信息
void BluetoothModule::printDeviceInfo() {
    if (pClient && pClient->isConnected()) {
        Serial.println("Device Information:");
        Serial.print("Device Address: ");
        Serial.println(pClient->getPeerAddress().toString().c_str());
    } else {
        Serial.println("No device connected.");
    }
}