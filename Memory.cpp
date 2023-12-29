#include "Memory.h"

Memory::Memory(fs::FS &fileSystem)
{
    this->fileSystem = &fileSystem;
    if(!this->fileSystem->exists(this->wifiConfigFile.c_str())){
        File file = this->fileSystem->open(this->wifiConfigFile.c_str(), FILE_WRITE);
        Serial.println("[Memory] Wifi config file created");
        file.close();
    }
}

File Memory::getFileFromPath(String path)
{
    try
    {
        return this->fileSystem->open(path, "r");
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
    String fileName;
    do {
        fileName = this->imagePreFileName + String(++this->imageFileNumber) + this->imageType;
    } while (this->fileSystem->exists(fileName));

    this->fileSystem->open(fileName, "w").close();
    Serial.printf("[Memory] New image file created: %s\n", fileName.c_str());
    return fileName;
}

String Memory::getImageFromPath(String path)
{
    File file = this->fileSystem->open(path.c_str(), "r");
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return "[Memory] File open failed";
    }

    String imageBase64 = file.readString();
    file.close();
    Serial.println("[Memory] File read and base64 conversion successful");
    return imageBase64;
}

void Memory::addNewImage(const String image, const String path)
{
    File file = this->fileSystem->open(path.c_str(), "w");
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return;
    }

    file.print(image);
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
    String wifiConfig = "\"" + ssid + "\":\"" + password + "\"";
    File historyFile = this->fileSystem->open(this->wifiConfigFile.c_str(), "r");
    String history = historyFile.readString();
    if(history.length() > 0){
        wifiConfig = history + "," + wifiConfig;
    }
    historyFile.close();

    File file = this->fileSystem->open(this->wifiConfigFile.c_str(), "w");
    if (file)
    {
        file.print(wifiConfig);
        file.close();
        Serial.printf("[Memory] The wifi config has been saved in %s - Size: %d bytes\n", this->wifiConfigFile.c_str(), file.size());
    }
    else
    {
        Serial.println("[Memory] File open failed");
    }



    Serial.printf("[Memory] The wifi config has been saved in %s - Size: %d bytes\n", this->wifiConfigFile.c_str(), file.size());
}

String Memory::getWifiPass(const String ssid)
{
    File file = this->fileSystem->open(this->wifiConfigFile.c_str(), "r");
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return "";
    }

    String fileContent = "{" + file.readString() + "}";


    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, fileContent);
    if (error)
    {
        Serial.println("[Memory] Failed to read file, using default configuration");
        return "";
    }

    file.close();

    if(doc.containsKey(ssid)){
        return doc[ssid];
    }
    else{
        return "";
    }
}

StaticJsonDocument<200> Memory::getWifiConfigs()
{
    File file = this->fileSystem->open(this->wifiConfigFile.c_str(), "r");
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return JsonObject();
    }

    String fileContent = "{" + file.readString() + "}";
    file.close();

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, fileContent);
    if (error)
    {
        Serial.println("[Memory] Failed to read file, using default configuration");
    }

    return doc;
}

void Memory::deleteWifiConfig(const String ssid)
{
    String fileContent = "";
    StaticJsonDocument<200> doc;
    File file = this->fileSystem->open(this->wifiConfigFile.c_str(), "r");
    if (!file)
    {
        Serial.println("[Memory] File open failed");
        return;
    }

    fileContent = "{" + file.readString() + "}";
    DeserializationError error = deserializeJson(doc, fileContent);
    if (error)
    {
        Serial.println("[Memory] Failed to read file, using default configuration");
        return;
    }
    file.close();

    JsonObject obj = doc.as<JsonObject>();
    obj.remove(ssid);

    file = this->fileSystem->open(this->wifiConfigFile.c_str(), "w");
    serializeJson(doc, fileContent);
    if (!file)
    {
        file.print(fileContent);
        Serial.println("[Memory] File open failed");
        return;
    }
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






