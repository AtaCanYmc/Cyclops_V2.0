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

// https://mischianti.org/how-to-create-a-rest-server-on-esp8266-and-esp32-startup-part-1/


//--------------------------------- Camera variables

#define CAMERA_MODEL_AI_THINKER

//--------------------------------- WIFI variables

const char* ssid = "Kablonet Netmaster-0720-G";
const char* pwd = "6db100b8";
IPAddress ip;

WebServer server(80);
Brain brain(server, SPIFFS);


void setup() {
    Serial.println("[Cyclops] Cyclops V2.0");

    Serial.begin(115200);

    scanWifi();
    connectWifi(ssid,pwd);
    setMDNS();

    // set device time from NTP servers for Turkey time zone
    setDeviceTime();

    // start SPIFFS
    startSPIFFS();

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
  int numberOfNetworks = WiFi.scanNetworks();

  Serial.println("[Wifi] ----------------------- Scanning Networks");

  for(int i = 0; i < numberOfNetworks; i++){
    Serial.println("\n--------------------------------");
    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));
    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));
    Serial.println("--------------------------------");
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


