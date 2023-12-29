#ifndef Eye_h
#define Eye_h

#include "esp_camera.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include "Base64.h"
#include <FS.h>
#include "Memory.h"

#define FLASH_PIN 4

//------------------ ESP32 CAM ------------------

class Eye { // ESP-CAM for image capture

public:
    bool isFlashOnCapture = false;
    bool flashState = false;
    bool autoSave = false;
    Memory *memory;

    Eye(Memory &memory);
    String capture();
    String save(String base64Image);
    void setFlash(boolean flashState);
    bool check(String fileName);
private:

};

#endif
