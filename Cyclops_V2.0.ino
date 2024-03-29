#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <SPIFFS.h>
#include <FS.h>
#include "esp_camera.h"
#include "camera_pins.h"
#include "camera_code.h"
#include "Brain.h"

//--------------------------------- Camera variables

#define CAMERA_MODEL_AI_THINKER

//--------------------------------- WIFI variables

const char* ssid = "[Your SSID]";
const char* pwd = "[Your Password]";
IPAddress ip;

WebServer server(80);
Memory memory(SPIFFS);
Brain brain(server, memory);

void setup() {
    Serial.println("[Cyclops] Cyclops V2.0");
    Serial.begin(115200);

    // start SPIFFS
    startSPIFFS();

    // start WIFI
    scanWifi();
    setMDNS();

    // set device time from NTP servers for Turkey time zone
    setDeviceTime();

    // start camera
    startESPCamera();

    // start web server
    brain.restServerRouting();
}

void loop() {
    server.handleClient();
}

//--------------------------------- WIFI

void scanWifi(){
    bool wifiConnected = false;
    int numberOfNetworks = WiFi.scanNetworks();
    Serial.println("[Wifi] ----------------------- Scanning Networks");

    for(int i = 0; i < numberOfNetworks; i++){
        Serial.println("\n--------------------------------");
        Serial.print("Network name: ");
        Serial.println(WiFi.SSID(i));
        Serial.print("Signal strength: ");
        Serial.println(WiFi.RSSI(i));
        Serial.println("--------------------------------");

        if(memory.getWifiPass(WiFi.SSID(i)) != ""){
            Serial.println("[Wifi] This network is already saved");
            connectWifi(WiFi.SSID(i).c_str(), memory.getWifiPass(WiFi.SSID(i)).c_str());
            wifiConnected = true;
            break;
        }
    }

    if(!wifiConnected){
        Serial.println("[Wifi] No saved network found");
        Serial.println("[Wifi] Connecting to default network");
        connectWifi(ssid,pwd);
    }
    Serial.println("\n[Wifi] ----------------------- End of Networks\n");
}

void connectWifi(const char* ssid, const char* pwd){
    Serial.print("[Wifi] Connection found on: ");
    Serial.print(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    Serial.print("\n[Wifi] Connecting to WiFi ..");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(500);
    }
    Serial.println("\n[Wifi] Wifi Connected");

    Serial.print("[Wifi] RRSI: ");
    Serial.println(WiFi.RSSI());
    ip = WiFi.localIP();
    Serial.print("[Wifi] IP ADDRESS: ");
    Serial.println(ip);
    Serial.println("\n");
}

void disconnectWifi(){
    WiFi.disconnect();
    Serial.println("[Wifi] Disconnected\n");
}

void setMDNS(){
    int totalTry = 5;
    while (!MDNS.begin("cyclops") && totalTry > 0) {
        Serial.println(".");
        delay(1000);
        totalTry--;
    }

    Serial.println("[Wifi] mDNS responder started");
    Serial.println("[Wifi] You can now connect to http://cyclops.local\n");
}

// --------------------------------- Time

void setDeviceTime() {
    // Set device time from NTP servers for Turkey time zone
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    // Wait for time to be set
    time_t now = time(nullptr);
    if (now > 0) {
        Serial.println("[Time] Device time set successfully.");
    } else {
        Serial.println("[Time] Failed to set device time.");
    }
}

String getDateTime(){
    char dateTime[30];
    time_t now = time(nullptr);
    struct tm * timeinfo;
    timeinfo = localtime(&now);
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", timeinfo);
    return String(dateTime);
}

// --------------------------------- Camera

void startESPCamera(){
    int totalTry = 5;
    while (!setupCamera() && totalTry > 0) {
        Serial.println("[Eye] Camera Setup Failed! Retrying...");
        totalTry--;
    }
    if (totalTry == 0) {
        Serial.println("[Eye] Camera Setup Failed!");
        return;
    }
    Serial.println("[Eye] Camera Setup Success!");
}

void startSPIFFS(){
    if (!SPIFFS.begin(true)) {
        Serial.println("[File System] An Error has occurred while mounting SPIFFS");
        return;
    }
    else {
        delay(500);
        Serial.println("[File System] SPIFFS mounted successfully");
    }
}


