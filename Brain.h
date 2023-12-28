#ifndef Brain_h
#define Brain_h

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WebServer.h>
#include <mDNS.h>
#include <ArduinoJson.h>
#include "Base64.h"
#include "Skeleton.h"
#include "Memory.h"
#include "Eye.h"
#include <SPIFFS.h>
#include <FS.h>

struct errorContainer {
    String date;
    String error;
    String errorType;
    int errorCode;
};

class Brain { // For Web Server
    public:
        WebServer *brainServer;
        Memory *memory;
        Skeleton *skeleton;
        Eye *eye;

        Brain(WebServer &brainServer, Memory &memory);
        void restServerRouting();
        String getDateTime();

    private:
        void restartDevice();
        void handleNotFound();
        void handleRoot();
        void returnError(errorContainer error);

        void getWifiScan();
        void setWifiConnection();
        void connectToWifi(String ssid, String password);
        void addWifiConfig();

        void getEye();
        void setEye();

        void getSkeleton();
        void setSkeleton();

        void captureImage();
};

#endif
