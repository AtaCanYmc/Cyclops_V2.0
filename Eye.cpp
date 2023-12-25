#include "Eye.h"

Eye::Eye(fs::FS &fileSystem) {
    this->fileSystem = &fileSystem;
    pinMode(FLASH_PIN, OUTPUT);
    digitalWrite(FLASH_PIN, LOW);
}

String Eye::getFileName(int fileNumber) {
    String fileName = preFileName + String(fileNumber) + postFileName;
    return fileName;
}

void Eye::flashToggle() {
    flashState = !flashState;
    digitalWrite(FLASH_PIN, flashState);
}

void Eye::setFlash(boolean flashState) {
    this->flashState = flashState;
    digitalWrite(FLASH_PIN, flashState);
}

String Eye::savePhoto(fs::FS &fs) {
    this->fb = NULL;
    bool ok = 0;
    int maxTry = 5;
    int fNumber = this->fileNumber;
    String fileName = this->getFileName(fNumber);

    do {
        Serial.println("[Eye] Taking a photo...");
        if(this->isFlashOnCapture) {
            digitalWrite(FLASH_PIN, HIGH);
        }

        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("[Eye] Camera capture failed");
            return "Camera capture failed";
        } else {
            Serial.println("[Eye] Camera capture successful");
            digitalWrite(FLASH_PIN, flashState);
        }



        Serial.printf("[EYE] Picture file name: %s\n", fileName.c_str());
        File file = SPIFFS.open(fileName.c_str(), FILE_WRITE);

        if (!file) {
            Serial.println("[EYE] Failed to open file in writing mode");
        }
        else {
            file.write(fb->buf, fb->len); // payload (image), payload length
            Serial.print("[EYE] The picture has been saved in ");
            Serial.print(fileName);
            Serial.print(" - Size: ");
            Serial.print(file.size());
            Serial.println(" bytes");
        }

        file.close();
        esp_camera_fb_return(fb);

        ok = checkPhoto(SPIFFS);
    } while ( !ok && maxTry-- > 0 );

    this->fileNumber++;
    return fileName;
}

bool Eye::checkPhoto(fs::FS &fs) {
    File f_pic = fs.open( FILE_PATH );
    unsigned int pic_sz = f_pic.size();
    return ( pic_sz > 100 );
}

File Eye::getPhoto(fs::FS &fs) {
    File f_pic = fs.open(FILE_PATH, FILE_READ);
    return f_pic;
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

 String Eye::getPhotoBase64(int fileNumber, fs::FS &fs) {
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
}

void Eye::deleteImage(int fileNumber, fs::FS &fs) {
    String fileName = this->getFileName(fileNumber);
    while(fs.exists(fileName.c_str())) {
        fs.remove(fileName.c_str());
        Serial.println("[Eye] File deleted");
    }
}

int Eye::getLastFileNumber(fs::FS &fs) {
    int fileNumber = 0;
    while(fs.exists(this->getFileName(fileNumber).c_str())) {
        fileNumber++;
    }
    return fileNumber;
}


