#ifndef CYCLOPS_V2_0_MEMORY_H
#define CYCLOPS_V2_0_MEMORY_H

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "Base64.h"
#include <ArduinoJson.h>

class Memory {
    public:
        fs::FS *fileSystem;

        Memory(fs::FS &fileSystem);
        File getFileFromPath(String path);

        String requestNewImagePath();
        String getImageFromPath(String path);
        void addNewImage(String image, String path);
        void deleteImage(String path);
        void deleteAllImages();

        void addNewWifiConfig(String ssid, String password);
        JsonObject getWifiConfig(String ssid);
        JsonObject getAllWifiConfig();
        void deleteWifiConfig(String ssid);
        void deleteAllWifiConfig();

    private:
        int imageFileNumber = 0;
        String imagePreFileName = "Images/cyclops_";
        String imageType = ".jpg";
        String wifiConfigFile = "/wifiConfig.json";
};

#endif //CYCLOPS_V2_0_MEMORY_H
