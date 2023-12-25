#include "Memory.h"

Memory::Memory(fs::FS &fileSystem)
{
    this->fileSystem = &fileSystem;
    if(!this->fileSystem->exists(this->wifiConfigFile.c_str())){
        File file = this->fileSystem->open(this->wifiConfigFile.c_str(), FILE_WRITE);
        file.close();
    }
    if(!this->fileSystem->exists("/Images")){
        this->fileSystem->mkdir("/Images");
    }
}

File Memory::getFileFromPath(String path)
{
    try
    {
        return this->fileSystem->open(path);
    }
    catch (const std::exception &e)
    {
        Serial.println("[Memory] Error opening file");
        Serial.println(e.what());
        return File();
    }
}

String Memory::requestNewImagePath()
{
    String fileName = this->imagePreFileName + String(this->imageFileNumber) + this->imageType;
    this->imageFileNumber++;
    return fileName;
}

String Memory::getImageFromPath(String path)
{
    File file = this->getFileFromPath(path);
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return "";
    }

    size_t fileSize = file.size();
    std::unique_ptr<uint8_t[]> buf(new uint8_t[fileSize]);
    file.read(buf.get(), fileSize);

    char *input = (char *)buf.get();
    char output[base64_enc_len(3)];
    String imageFile = "data:image/jpeg;base64,";

    for (size_t i = 0; i < fileSize; i++)
    {
        base64_encode(output, (input++), 3);
        if (i % 3 == 0)
            imageFile += String(output);
    }

    file.close();

    Serial.println("[Memory] File read and base64 conversion successful");
    return imageFile;
}

void Memory::addNewImage(const String image, const String path)
{
    File file = this->getFileFromPath(path);
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return;
    }

    int inputLength = image.length();
    int decodedLength = base64_dec_len((char*)image.c_str(), inputLength);
    char decoded[decodedLength];
    base64_decode(decoded, (char*)image.c_str(), inputLength);

    file.write((uint8_t *)decoded, decodedLength);
    file.close();

    Serial.printf("[Memory] The picture has been saved in %s - Size: %d bytes\n", path.c_str(), file.size());
}

void Memory::deleteImage(const String path)
{
    while (this->fileSystem->exists(path.c_str()))
    {
        this->fileSystem->remove(path.c_str());
        Serial.println("[Memory] File deleted");
    }
}

void Memory::deleteAllImages()
{
    if(this->fileSystem->exists("/Images")){
        this->fileSystem->rmdir("/Images");
        Serial.println("[Memory] Images folder deleted");
        // create a new one
        this->fileSystem->mkdir("/Images");
    }
    else{
        Serial.println("[Memory] Images folder does not exist");
    }
}

void Memory::addNewWifiConfig(const String ssid, const String password)
{
    String wifiConfig = "{\"" + ssid + "\":\"" + password + "\"}";
    File file = this->getFileFromPath(this->wifiConfigFile);
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return;
    }

    file.write((uint8_t *)wifiConfig.c_str(), wifiConfig.length());
    file.close();

    Serial.printf("[Memory] The wifi config has been saved in %s - Size: %d bytes\n", this->wifiConfigFile.c_str(), file.size());
}

JsonObject Memory::getWifiConfig(const String ssid)
{
    File file = this->getFileFromPath(this->wifiConfigFile);
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return JsonObject();
    }

    size_t fileSize = file.size();
    std::unique_ptr<uint8_t[]> buf(new uint8_t[fileSize]);
    file.read(buf.get(), fileSize);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    if (error)
    {
        Serial.println("[Memory] Failed to read file, using default configuration");
        return JsonObject();
    }

    file.close();

    return doc.as<JsonObject>();
}

JsonObject Memory::getAllWifiConfig()
{
    File file = this->getFileFromPath(this->wifiConfigFile);
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return JsonObject();
    }

    size_t fileSize = file.size();
    std::unique_ptr<uint8_t[]> buf(new uint8_t[fileSize]);
    file.read(buf.get(), fileSize);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    if (error)
    {
        Serial.println("[Memory] Failed to read file, using default configuration");
        return JsonObject();
    }

    file.close();

    return doc.as<JsonObject>();
}

void Memory::deleteWifiConfig(const String ssid)
{
    File file = this->getFileFromPath(this->wifiConfigFile);
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return;
    }

    size_t fileSize = file.size();
    std::unique_ptr<uint8_t[]> buf(new uint8_t[fileSize]);
    file.read(buf.get(), fileSize);

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    if (error)
    {
        Serial.println("[Memory] Failed to read file, using default configuration");
        return;
    }

    file.close();

    JsonObject obj = doc.as<JsonObject>();
    obj.remove(ssid);

    file = this->getFileFromPath(this->wifiConfigFile);
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return;
    }

    serializeJson(doc, file);
    file.close();
}

void Memory::deleteAllWifiConfig()
{
    this->fileSystem->remove(this->wifiConfigFile.c_str());
    Serial.println("[Memory] Wifi config deleted");
    // open new again
    File file = this->fileSystem->open(this->wifiConfigFile.c_str(), FILE_WRITE);
    file.close();
}






