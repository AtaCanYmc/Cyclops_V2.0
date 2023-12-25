#include "Brain.h"

Brain::Brain(WebServer &brainServer, fs::FS &fileSystem){
    this->fileSystem = &fileSystem;
    this->brainServer = &brainServer;
    this->skeleton = new Skeleton();
    this->eye = new Eye(*this->fileSystem);
}

void Brain::restartDevice(){
    String messageJson;
    StaticJsonDocument<512> doc;
    doc["date"] = getDateTime();
    doc["message"] = "Device is restarting";
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
    uint32_t t = millis();
    if (t + 1000 < millis()){
        ESP.restart();
    }
}

void Brain::restServerRouting(){
    Serial.println("[Brain] Rest Server Routing");

    brainServer->on("/", HTTP_GET, [&](){
        handleRoot();
    });

    brainServer->on("/settings", HTTP_GET, [&](){
        getSettings();
    });

    brainServer->on("/wifiScan", HTTP_GET, [&](){
        getWifiScan();
    });

    brainServer->on("/wifi", HTTP_POST, [&](){
        setWifi();
    });

    brainServer->on("/eye", HTTP_GET, [&](){
        getEye();
    });

    brainServer->on("/eye", HTTP_POST, [&](){
        setEye();
    });

    brainServer->on("/skeleton", HTTP_GET, [&](){
        getSkeleton();
    });

    brainServer->on("/skeleton", HTTP_POST, [&](){
        setSkeleton();
    });

    brainServer->on("/restart", HTTP_POST, [&](){
        restartDevice();
    });

    brainServer->on("/capture", HTTP_GET, [&](){
        captureImage();
    });

    brainServer->onNotFound([&](){
        handleNotFound();
    });

    brainServer->begin();
}

void Brain::handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += brainServer->uri();
    message += "\nMethod: ";
    message += (brainServer->method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += brainServer->args();
    message += "\n";

    for (uint8_t i=0; i<brainServer->args(); i++){
        message += " " + brainServer->argName(i) + ": " + brainServer->arg(i) + "\n";
    }
    brainServer->send(404, "text/plain", message);
}

void Brain::handleRoot() {
    String messageJson;
    StaticJsonDocument<512> doc;
    doc["message"] = "Hello from Cyclops";
    doc["version"] = "2.0";
    doc["author"] = "AtaCanYmc";
    doc["date"] = getDateTime();
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::returnError(errorContainer error){
    String errorJson;
    StaticJsonDocument<200> doc;
    doc["date"] = error.date;
    doc["error"] = error.error;
    doc["errorType"] = error.errorType;
    doc["errorCode"] = error.errorCode;
    serializeJson(doc, errorJson);
    brainServer->send(200, "application/json", errorJson);
}

void Brain::getSettings(){
    String settingsJson;
    StaticJsonDocument<1024> doc;
    doc["date"] = getDateTime();

    doc["deviceInfo"]["name"] = "Cyclops";
    doc["deviceInfo"]["version"] = "2.0";
    doc["deviceInfo"]["author"] = "AtaCanYmc";
    doc["deviceInfo"]["horizontaPIN"] = HORIZONTAL_PIN;
    doc["deviceInfo"]["verticalPIN"] = VERTICAL_PIN;
    doc["deviceInfo"]["isFlashOnCapture"] = eye->isFlashOnCapture;
    doc["deviceInfo"]["isFlashOn"] = eye->flashState;
    doc["deviceInfo"]["horizontalAxisAngle"] = skeleton->horizontalAxisAngle;
    doc["deviceInfo"]["verticalAxisAngle"] = skeleton->verticalAxisAngle;

    doc["chipInfo"]["chipModel"] = ESP.getChipModel();
    doc["chipInfo"]["chipRevision"] = ESP.getChipRevision();
    doc["chipInfo"]["cpuFreqMHz"] = ESP.getCpuFreqMHz();

    doc["wifiInfo"]["ssid"] = WiFi.SSID();
    doc["wifiInfo"]["pwd"] = WiFi.psk();
    doc["wifiInfo"]["rssi"] = WiFi.RSSI();
    doc["wifiInfo"]["ip"] = WiFi.localIP().toString();
    doc["wifiInfo"]["isConnected"] = WiFi.isConnected();
    doc["wifiInfo"]["host"] = WiFi.getHostname();
    doc["wifiInfo"]["dns"] = WiFi.dnsIP().toString();
    doc["wifiInfo"]["mac"] = WiFi.macAddress();

    serializeJson(doc, settingsJson);
    brainServer->send(200, "application/json", settingsJson);
}

void Brain::getWifiScan(){
    String wifiScanJson;
    StaticJsonDocument<1024> doc;
    StaticJsonDocument<1024> wifis;
    int numberOfNetworks = WiFi.scanNetworks();
    
    doc["date"] = getDateTime();
    doc["wifiCount"] = numberOfNetworks;

    for(int i = 0; i < numberOfNetworks; i++){
        wifis[i]["ssid"] = WiFi.SSID(i);
        wifis[i]["rssi"] = WiFi.RSSI(i);
    }
    doc["wifis"] = wifis;
    serializeJson(doc, wifiScanJson);
    brainServer->send(200, "application/json", wifiScanJson);
}

String Brain::getDateTime(){
    char dateTime[30];
    time_t now = time(nullptr);
    struct tm * timeinfo;
    timeinfo = localtime(&now);
    strftime(dateTime, sizeof(dateTime), "%d-%m-%Y %H:%M:%S", timeinfo);
    return String(dateTime);
}

void Brain::setSettings(){ // TODO: add body read
    String messageJson;
    StaticJsonDocument<512> doc;
    doc["message"] = "Settings updated";
    doc["date"] = getDateTime();
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::setWifi(){  // TODO: add body read
    String messageJson;
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    String postBody = brainServer->arg("plain");
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Wifi] deserializeJson() failed: "));
        doc["date"] = getDateTime();
        doc["message"] = "Wifi update failed";
        doc["error"] = error.c_str();
        brainServer->send(400, "application/json", messageJson);
        return;
    }

    JsonObject postObj = postBodyDoc.as<JsonObject>();
    const char* ssid = postObj["ssid"];
    const char* pwd = postObj["pwd"];
    //connectWifi(ssid, pwd);

    doc["message"] = "Wifi updated";
    doc["date"] = getDateTime();
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::getEye(){
    String messageJson;
    StaticJsonDocument<512> doc;
    doc["date"] = getDateTime();
    doc["isFlashOnCapture"] = eye->isFlashOnCapture;
    doc["isFlashOn"] = eye->flashState;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::setEye(){ // TODO: add body read
    String messageJson;
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    String postBody = brainServer->arg("plain");
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Eye] deserializeJson() failed: "));
        doc["date"] = getDateTime();
        doc["message"] = "Eye update failed";
        doc["error"] = error.c_str();
        brainServer->send(400, "application/json", messageJson);
        return;
    }

    JsonObject postObj = postBodyDoc.as<JsonObject>();
    eye->setFlash(postObj["isFlashOn"]);
    eye->isFlashOnCapture = postObj["isFlashOnCapture"];

    doc["date"] = getDateTime();
    doc["message"] = "Eye updated";
    doc["isFlashOnCapture"] = eye->isFlashOnCapture;
    doc["isFlashOn"] = eye->flashState;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::getSkeleton(){
    String messageJson;
    StaticJsonDocument<512> doc;
    doc["date"] = getDateTime();
    doc["horizontalAxisAngle"] = skeleton->horizontalAxisAngle;
    doc["verticalAxisAngle"] = skeleton->verticalAxisAngle;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::setSkeleton(){ // TODO: add body read
    String postBody = brainServer->arg("plain");
    Serial.println(postBody);
    String messageJson;
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Skeleton] deserializeJson() failed: "));
        doc["date"] = getDateTime();
        doc["message"] = "Skeleton update failed";
        doc["error"] = error.c_str();
        brainServer->send(400, "application/json", messageJson);
        return;
    }

    JsonObject postObj = postBodyDoc.as<JsonObject>();
    skeleton->setAxis('X', postObj["horizontalAxisAngle"]);
    skeleton->setAxis('Y', postObj["verticalAxisAngle"]);

    doc["date"] = getDateTime();
    doc["message"] = "Skeleton updated";
    doc["horizontalAxisAngle"] = skeleton->horizontalAxisAngle;
    doc["verticalAxisAngle"] = skeleton->verticalAxisAngle;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::captureImage(){
    String imagePath = this->eye->savePhoto(*this->fileSystem);
    bool isOk = this->eye->checkPhoto(*this->fileSystem);
    if(!isOk){
        errorContainer error;
        error.date = getDateTime();
        error.error = "Image capture failed";
        error.errorType = "Image";
        error.errorCode = 501;
        returnError(error);
        return;
    }
    String base64Image = this->eye->getPhotoBase64(this->eye->fileNumber - 1, *this->fileSystem);
    String messageJson;
    DynamicJsonDocument doc(1024*15);

    doc["date"] = getDateTime();
    doc["message"] = isOk ? "Image captured" : "Image capture failed";
    doc["isFlashOn"] = eye->flashState;
    doc["isFlashOnCapture"] = eye->isFlashOnCapture;
    doc["imageType"] = "image/jpeg";
    doc["imageSize"] = base64Image.length();
    doc["imagePath"] = imagePath;
    doc["image"] = base64Image;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}










