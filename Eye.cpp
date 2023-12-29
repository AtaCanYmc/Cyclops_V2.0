#include "Eye.h"

Eye::Eye(Memory &memory) {
    this->memory = &memory;
    pinMode(FLASH_PIN, OUTPUT);
    digitalWrite(FLASH_PIN, LOW);
}

void Eye::setFlash(boolean flashState) {
    this->flashState = flashState;
    digitalWrite(FLASH_PIN, flashState);
}

String Eye::capture() {
    esp_camera_fb_return(esp_camera_fb_get());
    camera_fb_t * fb = NULL;
    String imageFile = "data:image/jpeg;base64,";

    Serial.println("[Eye] Taking a photo...");
    if(this->isFlashOnCapture) {
        this->setFlash(true);
        delay(100);
    }
    fb = esp_camera_fb_get();
    this->setFlash(false);

    if (!fb) {
        Serial.println("[Eye] Camera capture failed");
        esp_camera_fb_return(fb);
        return "";
    }

    char *input = (char *)fb->buf;
    char output[base64_enc_len(3)];

    for (int i=0;i<fb->len;i++) {
        base64_encode(output, (input++), 3);
        if (i%3==0) imageFile += String(output);
    }

    esp_camera_fb_return(fb);

    Serial.println("Camera capture successful");
    //Serial.println(imageFile);
    return imageFile;
}

String Eye::save(String base64Image) {
    String fileName = this->memory->requestNewImagePath();
    this->memory->addNewImage(base64Image, fileName);
    return fileName;
}

bool Eye::check(String fileName) {
    File file = this->memory->fileSystem->open(fileName, "r");
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return false;
    }
    unsigned int fileSize = file.size();
    file.close();
    return (fileSize > 100);
}

