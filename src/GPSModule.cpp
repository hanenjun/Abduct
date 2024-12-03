#include "GPSModule.h"

GPSModule::GPSModule(int RX_PIN, int TX_PIN, int PPS_PIN) 
    : mySerial(RX_PIN, TX_PIN), _PPS_PIN(PPS_PIN) { // 初始化串口
    // 这里不传递 PPS_PIN 到串口构造函数
}
void GPSModule::initGPS(long baudRate, int RX_PIN, int TX_PIN) {
    mySerial.begin(baudRate, SWSERIAL_8N1, RX_PIN, TX_PIN);  // Initialize serial communication with GPS
    delay(3000);  // Wait for GPS module to initialize
    Serial.println("ATGM336H GPS Module Test");
}

void GPSModule::update() {
    while (mySerial.available()) {
        char c = mySerial.read();
        nmeaData += c;  // Collect each character

        if (c == '\n') {
            parseNMEAData(nmeaData);  // Parse the NMEA data
            nmeaData = "";  // Clear the buffer for the next data
        }
    }
}

void GPSModule::parseNMEAData(String data) {
    if (data.startsWith("$GPGGA")) {
        parseGGA(data);  // Parse GGA sentence
    } else if (data.startsWith("$GPGSV")) {
        parseGSV(data);  // Parse GSV sentence
    } else if (data.startsWith("$GPGSA")) {
        parseGSA(data);  // Parse GSA sentence
    }
}

void GPSModule::parseGGA(String ggaData) {
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

void GPSModule::parseGSV(String gsvData) {
    int numSatellites = 0;
    int startIndex = gsvData.indexOf(",") + 1;
    int endIndex = gsvData.indexOf(",", startIndex);
    numSatellites = gsvData.substring(startIndex, endIndex).toInt();

    Serial.print("Number of Satellites in view: ");
    Serial.println(numSatellites);

    for (int i = 0; i < numSatellites; i++) {
        int satelliteIndex = gsvData.indexOf(",", endIndex) + 1;
        endIndex = gsvData.indexOf(",", satelliteIndex);
        String satelliteID = gsvData.substring(satelliteIndex, endIndex);

        satelliteIndex = gsvData.indexOf(",", endIndex) + 1;
        endIndex = gsvData.indexOf(",", satelliteIndex);
        String elevation = gsvData.substring(satelliteIndex, endIndex);  // Satellite elevation

        satelliteIndex = gsvData.indexOf(",", endIndex) + 1;
        endIndex = gsvData.indexOf(",", satelliteIndex);
        String azimuth = gsvData.substring(satelliteIndex, endIndex);  // Satellite azimuth

        satelliteIndex = gsvData.indexOf(",", endIndex) + 1;
        endIndex = gsvData.indexOf("*", satelliteIndex);
        String signalStrength = gsvData.substring(satelliteIndex, endIndex);  // Signal strength

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

void GPSModule::parseGSA(String gsaData) {
    int modeStart = gsaData.indexOf(",") + 1;
    int modeEnd = gsaData.indexOf(",", modeStart);
    String mode = gsaData.substring(modeStart, modeEnd);  // Mode: 1 = no fix, 2 = 2D fix, 3 = 3D fix

    int fixTypeStart = gsaData.indexOf(",", modeEnd + 1) + 1;
    int fixTypeEnd = gsaData.indexOf(",", fixTypeStart);
    String fixType = gsaData.substring(fixTypeStart, fixTypeEnd);  // Fix type

    Serial.print("Fix Status: ");
    Serial.println(fixType);
}



// GPSModule gps;  // Create GPS object with RX, TX, and PPS pins

// void setup() {
//   Serial.begin(115200);  // Initialize the serial monitor for debugging output
//   gps.begin(9600);  // Initialize GPS module with default baud rate 9600
// }

// void loop() {
//   gps.update();  // Continuously update and parse GPS data
// }