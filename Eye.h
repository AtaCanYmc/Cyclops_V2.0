#ifndef Eye_h
#define Eye_h

#include "esp_camera.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include "Base64.h"
#include <FS.h>

#define FLASH_PIN 4
#define FILE_PATH "/image.jpg"

//------------------ ESP32 CAM ------------------

class Eye { // ESP-CAM for image capture

public:
    bool isFlashOnCapture = false;
    bool flashState = false;
    int fileNumber = 0;
    String preFileName = "/cyclops_";
    String postFileName = ".jpg";
    camera_fb_t *fb = NULL;
    fs::FS *fileSystem;

    Eye(fs::FS &fileSystem);
    String getFileName(int fileNumber);
    void flashToggle();
    void setFlash(boolean flashState);
    String savePhoto(fs::FS &fs);
    bool checkPhoto(fs::FS &fs);
    File getPhoto(fs::FS &fs);
    String getPhotoBase64(int fileNumber, fs::FS &fs);
    void deleteImage(int fileNumber, fs::FS &fs);
    int getLastFileNumber(fs::FS &fs);
private:

};

#endif
