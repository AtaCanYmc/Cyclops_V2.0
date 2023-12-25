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
#include "Eye.h"
#include <SPIFFS.h>
#include <FS.h>

#define HORIZONTAL_PIN 1
#define VERTICAL_PIN 2

struct errorContainer {
    String date;
    String error;
    String errorType;
    int errorCode;
};

class Brain { // For Web Server

public:
    WebServer *brainServer;
    fs::FS *fileSystem;
    Skeleton *skeleton;
    Eye *eye;

    Brain(WebServer &brainServer, fs::FS &fileSystem);
    void restServerRouting();
    String getDateTime();
private:
    void restartDevice();
    void handleNotFound();
    void handleRoot();
    void returnError(errorContainer error);

    void getSettings();
    void setSettings();

    void getWifiScan();
    void setWifi();

    void getEye();
    void setEye();

    void getSkeleton();
    void setSkeleton();

    void captureImage();
};

#endif
