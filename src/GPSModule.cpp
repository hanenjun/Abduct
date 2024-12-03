#include "GPSModule.h"

GPSModule::GPSModule(int RX_PIN, int TX_PIN, int PPS_PIN)
    : mySerial(RX_PIN, TX_PIN), _PPS_PIN(PPS_PIN), lastUpdateTime(0), initTimeout(3000)
{
    pinMode(_PPS_PIN, INPUT);                                         // 设置 PPS 引脚为输入模式
    attachInterrupt(digitalPinToInterrupt(_PPS_PIN), ppsISR, RISING); // 设置 PPS 引脚触发中断

    // 初始化 GPS 模块的串口
}

void GPSModule::initGPS(long baudRate)
{
    mySerial.begin(baudRate);  // 初始化 GPS 串口，RX_PIN 和 TX_PIN 在构造时已经传递
    lastUpdateTime = millis(); // 记录初始化的起始时间
    Serial.println("ATGM336H GPS Module Test");
}
// PPS 中断服务例程
void GPSModule::ppsISR() {
    // 处理 PPS 信号，每当 PPS 信号到达时，会触发这个中断
    Serial.println("PPS signal received, GPS time synchronized.");
    // 可以在此处进行同步逻辑，例如更新时间戳、处理 GPS 时间等
}
void GPSModule::update()
{
    // 如果初始化超时，则不再继续
    if (millis() - lastUpdateTime > initTimeout)
    {
        Serial.println("GPS initialization timed out.");
        return;
    }

    while (mySerial.available())
    {
        char c = mySerial.read();
        nmeaData += c; // 收集字符数据

        if (c == '\n')
        {
            isState = true;
            parseNMEAData(nmeaData); // 解析 NMEA 数据
            nmeaData = "";           // 清空缓冲区，准备下一个数据
        }
    }
}

// 获取格式化的 GPS 时间
String GPSModule::getGPSTimeInfo()
{
    return gpsTime; // 返回保存的格式化时间
}
void GPSModule::parseNMEAData(String data)
{
    if (data.startsWith("$GPGGA"))
    {
        parseGGA(data); // 解析 GGA 语句
    }
    else if (data.startsWith("$GPGSV"))
    {
        parseGSV(data); // 解析 GSV 语句
    }
    else if (data.startsWith("$GPGSA"))
    {
        parseGSA(data); // 解析 GSA 语句
    }
    else if (data.startsWith("$GPRMC"))
    {
        parseRMC(data); // 解析 RMC 语句
    }
}

void GPSModule::parseGGA(String ggaData)
{
    int latStart = ggaData.indexOf(",") + 1;
    int latEnd = ggaData.indexOf(",", latStart);
    String latitude = ggaData.substring(latStart, latEnd);

    int lonStart = ggaData.indexOf(",", latEnd + 1) + 1;
    int lonEnd = ggaData.indexOf(",", lonStart);
    String longitude = ggaData.substring(lonStart, lonEnd);

    Serial.print("Latitude: ");
    Serial.print(latitude);
    Serial.print(" , Longitude: ");
    Serial.println(longitude);
}

void GPSModule::parseGSV(String gsvData)
{
    int numSatellites = 0;
    int startIndex = gsvData.indexOf(",") + 1;
    int endIndex = gsvData.indexOf(",", startIndex);
    numSatellites = gsvData.substring(startIndex, endIndex).toInt();

    Serial.print("Number of Satellites in view: ");
    Serial.println(numSatellites);

    for (int i = 0; i < numSatellites; i++)
    {
        int satelliteIndex = gsvData.indexOf(",", endIndex) + 1;
        endIndex = gsvData.indexOf(",", satelliteIndex);
        String satelliteID = gsvData.substring(satelliteIndex, endIndex);

        satelliteIndex = gsvData.indexOf(",", endIndex) + 1;
        endIndex = gsvData.indexOf(",", satelliteIndex);
        String elevation = gsvData.substring(satelliteIndex, endIndex);

        satelliteIndex = gsvData.indexOf(",", endIndex) + 1;
        endIndex = gsvData.indexOf(",", satelliteIndex);
        String azimuth = gsvData.substring(satelliteIndex, endIndex);

        satelliteIndex = gsvData.indexOf(",", endIndex) + 1;
        endIndex = gsvData.indexOf("*", satelliteIndex);
        String signalStrength = gsvData.substring(satelliteIndex, endIndex);

        Serial.print("Satellite ID: ");
        Serial.print(satelliteID);
        Serial.print(" Elevation: ");
        Serial.print(elevation);
        Serial.print(" Azimuth: ");
        Serial.print(azimuth);
        Serial.print(" Signal Strength: ");
        Serial.println(signalStrength);
    }
}

void GPSModule::parseGSA(String gsaData)
{
    int modeStart = gsaData.indexOf(",") + 1;
    int modeEnd = gsaData.indexOf(",", modeStart);
    String mode = gsaData.substring(modeStart, modeEnd);

    int fixTypeStart = gsaData.indexOf(",", modeEnd + 1) + 1;
    int fixTypeEnd = gsaData.indexOf(",", fixTypeStart);
    String fixType = gsaData.substring(fixTypeStart, fixTypeEnd);

    Serial.print("Fix Status: ");
    Serial.println(fixType);
}

void GPSModule::parseRMC(String rmcData)
{
    // 解析 RMC 语句，提取位置和时间信息
    int timeStart = rmcData.indexOf(",") + 1;
    int timeEnd = rmcData.indexOf(",", timeStart);
    String time = rmcData.substring(timeStart, timeEnd);

    // 格式化时间为 HH:MM:SS
    String hours = time.substring(0, 2);
    String minutes = time.substring(2, 4);
    String seconds = time.substring(4, 6);

    gpsTime = hours + ":" + minutes + ":" + seconds; // 保存为格式化的时间
    Serial.print("Time: ");
    Serial.println(gpsTime);
}
