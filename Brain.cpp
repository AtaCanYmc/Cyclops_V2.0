#include "Brain.h"

Brain::Brain(WebServer &brainServer, Memory &memory){
    this->memory = &memory;
    this->brainServer = &brainServer;
    this->skeleton = new Skeleton();
    this->eye = new Eye(*this->memory);
}

void Brain::restartDevice(){
    String messageJson;
    StaticJsonDocument<512> doc;
    doc["date"] = getDateTime();
    doc["message"] = "Device is restarting";
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
    uint32_t t = millis();
    while (millis() - t < 5000) {}
    ESP.restart();
}

void Brain::restServerRouting(){
    Serial.println("[Brain] Rest Server Routing");

    brainServer->on("/", HTTP_GET, [&](){ // root
        handleRoot();
    });

    brainServer->on("/wifiScan", HTTP_GET, [&](){
        getWifiScan();
    });

    brainServer->on("/wifi", HTTP_POST, [&](){
        setWifiConnection();
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
    String errorMessage = "File Not Found\n\n";
    errorContainer error;
    error.date = getDateTime();
    error.errorType = "Not found";
    error.errorCode = 404;

    for (uint8_t i=0; i<brainServer->args(); i++){
        errorMessage += " " + brainServer->argName(i) + ": " + brainServer->arg(i) + "\n";
    }

    error.error = errorMessage;
    returnError(error);
}

void Brain::handleRoot() {
    String messageJson;
    StaticJsonDocument<600> doc;
    doc["project"]["name"] = "Cyclops";
    doc["project"]["version"] = "2.0";
    doc["project"]["author"] = "AtaCanYmc";
    doc["project"]["date"] = getDateTime();

    doc["device"]["author"] = "AtaCanYmc";
    doc["device"]["horizontaPIN"] = HORIZONTAL_PIN;
    doc["device"]["verticalPIN"] = VERTICAL_PIN;
    doc["device"]["isFlashOnCapture"] = eye->isFlashOnCapture;
    doc["device"]["isFlashOn"] = eye->flashState;
    doc["device"]["horizontalAxisAngle"] = skeleton->horizontalAxisAngle;
    doc["device"]["verticalAxisAngle"] = skeleton->verticalAxisAngle;

    doc["chip"]["chipModel"] = ESP.getChipModel();
    doc["chip"]["chipRevision"] = ESP.getChipRevision();
    doc["chip"]["cpuFreqMHz"] = ESP.getCpuFreqMHz();

    doc["wifi"]["ssid"] = WiFi.SSID();
    doc["wifi"]["pwd"] = WiFi.psk();
    doc["wifi"]["rssi"] = WiFi.RSSI();
    doc["wifi"]["ip"] = WiFi.localIP().toString();
    doc["wifi"]["isConnected"] = WiFi.isConnected();
    doc["wifi"]["host"] = WiFi.getHostname();
    doc["wifi"]["dns"] = WiFi.dnsIP().toString();
    doc["wifi"]["mac"] = WiFi.macAddress();
    doc["wifi"]["gateway"] = WiFi.gatewayIP().toString();
    doc["wifi"]["subnet"] = WiFi.subnetMask().toString();

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
    brainServer->send(error.errorCode, "application/json", errorJson);
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
        wifis[i]["encryptionType"] = WiFi.encryptionType(i);
        if(this->memory->getWifiPass(WiFi.SSID(i)) != "" ){
            wifis[i]["isConfigured"] = true;
            wifis[i]["pwd"] = this->memory->getWifiPass(WiFi.SSID(i));
        }
        else{
            wifis[i]["isConfigured"] = false;
        }

    }
    doc["wifiList"] = wifis;
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

void Brain::setWifiConnection(){  // TODO: add body read
    String messageJson;
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    errorContainer errorCont;
    String postBody = brainServer->arg("plain");
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Wifi] deserializeJson() failed: "));
        errorCont.date = getDateTime();
        errorCont.error = "Wifi update failed";
        errorCont.errorType = "Wifi";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    JsonObject postObj = postBodyDoc.as<JsonObject>();
    const char* ssid = postObj["ssid"];
    const char* pwd = postObj["pwd"];
    connectToWifi(ssid, pwd);

    doc["message"] = "Wifi updated";
    doc["date"] = getDateTime();
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::connectToWifi(String ssid, String password) {
    if(WiFi.isConnected()){
        Serial.println("[Wifi] Already connected to a wifi");
        WiFi.disconnect();
        Serial.println("[Wifi] Disconnected from wifi");
    }

    Serial.print("[Wifi] Connecting to wifi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(500);
    }

    Serial.println("\n[Wifi] Wifi Connected");
    Serial.print("[Wifi] RRSI: ");
    Serial.println(WiFi.RSSI());
    Serial.print("[Wifi] IP ADDRESS: ");
    Serial.println(WiFi.localIP());
}

void Brain::addWifiConfig(){ // TODO: add body read
    String messageJson;
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    errorContainer errorCont;
    String postBody = brainServer->arg("plain");
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Wifi] deserializeJson() failed: "));
        errorCont.date = getDateTime();
        errorCont.error = "Wifi update failed";
        errorCont.errorType = "Wifi";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    JsonObject postObj = postBodyDoc.as<JsonObject>();
    const char* ssid = postObj["ssid"];
    const char* pwd = postObj["pwd"];
    this->memory->addNewWifiConfig(ssid, pwd);

    doc["message"] = "Wifi updated";
    doc["date"] = getDateTime();
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::getEye(){
    String messageJson;
    StaticJsonDocument<200> doc;
    doc["date"] = getDateTime();
    doc["isFlashOnCapture"] = eye->isFlashOnCapture;
    doc["isFlashOn"] = eye->flashState;
    doc["autoSave"] = eye->autoSave;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::setEye(){ // TODO: add body read
    String messageJson;
    errorContainer errorCont;
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    String postBody = brainServer->arg("plain");
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Eye] deserializeJson() failed: "));
        errorCont.date = getDateTime();
        errorCont.error = "Eye update failed";
        errorCont.errorType = "Eye";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }
    try{
        JsonObject postObj = postBodyDoc.as<JsonObject>();
        eye->setFlash(postObj["isFlashOn"]);
        eye->isFlashOnCapture = postObj["isFlashOnCapture"];
        eye->autoSave = postObj["autoSave"];
    }
    catch(const std::exception& e){
        errorCont.date = getDateTime();
        errorCont.error = "Eye update failed";
        errorCont.errorType = "Eye";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    doc["date"] = getDateTime();
    doc["isFlashOnCapture"] = eye->isFlashOnCapture;
    doc["isFlashOn"] = eye->flashState;
    doc["autoSave"] = eye->autoSave;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::getSkeleton(){
    String messageJson;
    StaticJsonDocument<512> doc;
    doc["date"] = getDateTime();
    doc["horizontalAxisAngle"] = skeleton->horizontalAxisAngle;
    doc["verticalAxisAngle"] = skeleton->verticalAxisAngle;
    doc["verticalPin"] = VERTICAL_PIN;
    doc["horizontalPin"] = HORIZONTAL_PIN;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::setSkeleton(){ // TODO: add body read
    errorContainer errorCont;
    String postBody = brainServer->arg("plain");
    Serial.println(postBody);
    String messageJson;
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Skeleton] deserializeJson() failed: "));
        errorCont.date = getDateTime();
        errorCont.error = "Skeleton update failed";
        errorCont.errorType = "Skeleton";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    try {
        JsonObject postObj = postBodyDoc.as<JsonObject>();
        skeleton->setAxis('X', postObj["horizontalAxisAngle"]);
        skeleton->setAxis('Y', postObj["verticalAxisAngle"]);
    } catch (const std::exception& e){
        errorCont.date = getDateTime();
        errorCont.error = "Skeleton update failed";
        errorCont.errorType = "Skeleton";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    doc["date"] = getDateTime();
    doc["message"] = "Skeleton updated";
    doc["horizontalAxisAngle"] = skeleton->horizontalAxisAngle;
    doc["verticalAxisAngle"] = skeleton->verticalAxisAngle;
    doc["verticalPin"] = VERTICAL_PIN;
    doc["horizontalPin"] = HORIZONTAL_PIN;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::captureImage(){
    String messageJson;
    errorContainer errorCont;
    DynamicJsonDocument doc(1024*15);
    String base64Image = "";
    String imagePath = "";
    bool isOk = true;

    base64Image = eye->capture();

    if(base64Image == ""){
        errorCont.date = getDateTime();
        errorCont.error = "Image capture failed";
        errorCont.errorType = "Image";
        errorCont.errorCode = 500;
        returnError(errorCont);
        return;
    }

    if(this->eye->autoSave){
        imagePath = this->eye->save(base64Image);
        isOk = this->eye->check(imagePath);
    }

    doc["date"] = getDateTime();
    doc["image"]["data"] = base64Image;
    doc["image"]["path"] = imagePath;
    doc["image"]["status"] = isOk;
    doc["Eye"]["isFlashOnCapture"] = eye->isFlashOnCapture;
    doc["Eye"]["isFlashOn"] = eye->flashState;
    doc["Eye"]["autoSave"] = eye->autoSave;
    doc["Eye"]["flashPin"] = FLASH_PIN;

    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}










