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
    this->fb = NULL;
    char *input = (char *)fb->buf;
    char output[base64_enc_len(3)];
    String imageFile = "data:image/jpeg;base64,";

    Serial.println("[Eye] Taking a photo...");
    if(this->isFlashOnCapture) {
        this->setFlash(true);
    }
    fb = esp_camera_fb_get();
    this->setFlash(false);

    if (!fb) {
        Serial.println("[Eye] Camera capture failed");
        esp_camera_fb_return(fb);
        return "";
    }

    for (int i=0;i<fb->len;i++) {
        base64_encode(output, (input++), 3);
        if (i%3==0) imageFile += String(output);
    }

    esp_camera_fb_return(fb);

    Serial.println("Camera capture successful");
    Serial.println(imageFile);
    return imageFile;
}

String Eye::save(String base64Image) {
    String fileName = this->memory->requestNewImagePath();
    File file = this->memory->getFileFromPath(fileName);
    if (!file) {
        Serial.println("[Memory] File open failed");
        return "";
    }

    int outputLength = base64_dec_len((char *)base64Image.c_str(), base64Image.length());
    std::unique_ptr<uint8_t[]> buf(new uint8_t[outputLength]);
    base64_decode((char *)buf.get(), (char *)base64Image.c_str(), base64Image.length());

    file.write((uint8_t*)buf.get(), outputLength);
    file.close();

    Serial.println("[Memory] File saved");
    return fileName;
}

bool Eye::check(String fileName) {
    File file = this->memory->getFileFromPath(fileName);
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return false;
    }

    unsigned int fileSize = file.size();
    return (fileSize > 100);
}








/*String Eye::getPhotoBase64(fs::FS &fs) {
    camera_fb_t * fb = NULL;
    fb = esp_camera_fb_get();
    if(!fb) {
        Serial.println("Camera capture failed");
        return "Camera capture failed";
    }

    char *input = (char *)fb->buf;
    char output[base64_enc_len(3)];
    String imageFile = "data:image/jpeg;base64,";
    for (int i=0;i<fb->len;i++) {
        base64_encode(output, (input++), 3);
        if (i%3==0) imageFile += String(output);
    }

    esp_camera_fb_return(fb);

    Serial.println("Camera capture successful");
    Serial.println(imageFile);
    return imageFile;
}*/

 /*String Eye::getPhotoBase64(int fileNumber, fs::FS &fs) {
    String fileName = this->getFileName(fileNumber);
    File file = fs.open(fileName.c_str(), FILE_READ);
    if(!file) {
        Serial.println("[Eye] File open failed");
        return "[Eye] File open failed";
    }

    size_t fileSize = file.size();
    std::unique_ptr<uint8_t[]> buf(new uint8_t[fileSize]);
    file.read(buf.get(), fileSize);

    char *input = (char *)buf.get();
    char output[base64_enc_len(3)];
    String imageFile = "data:image/jpeg;base64,";
    
    for (size_t i = 0; i < fileSize; i++) {
        base64_encode(output, (input++), 3);
        if (i % 3 == 0) imageFile += String(output);
    }

    file.close();

    Serial.println("[Eye] File read and base64 conversion successful");
    //Serial.println(imageFile);
    return imageFile;
}*/


