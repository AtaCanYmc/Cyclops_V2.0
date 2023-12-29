#include "Brain.h"

Brain::Brain(WebServer &brainServer, Memory &memory){
    this->memory = &memory;
    this->brainServer = &brainServer;
    this->skeleton = new Skeleton();
    this->eye = new Eye(*this->memory);
    this->voice = new Voice();
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

    brainServer->on("/wifi-scan", HTTP_GET, [&](){
        getWifiScan();
    });

    brainServer->on("/wifi", HTTP_GET, [&](){
        getWifiConfigs();
    });

    brainServer->on("/wifi", HTTP_DELETE, [&](){
        deleteWifiConfig();
    });

    brainServer->on("/wifi-config", HTTP_DELETE, [&](){
        deleteAllWifiConfig();
    });

    brainServer->on("/wifi", HTTP_POST, [&](){
        addWifiConfig();
    });

    brainServer->on("/connect", HTTP_POST, [&](){
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

    brainServer->on("/voice", HTTP_GET, [&](){
        getVoice();
    });

    brainServer->on("/voice", HTTP_POST, [&](){
        setVoice();
    });

    brainServer->on("/play", HTTP_POST, [&](){
        makeSound();
    });

    brainServer->on("/restart", HTTP_POST, [&](){
        restartDevice();
    });

    brainServer->on("/capture", HTTP_GET, [&](){
        captureImage();
    });

    brainServer->on("/image", HTTP_POST, [&](){
        getImageFromFile();
    });

    brainServer->on("/image", HTTP_DELETE, [&](){
        deleteImage();
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

void Brain::getWifiConfigs(){
    String messageJson;
    StaticJsonDocument<512> doc;
    doc["date"] = getDateTime();
    doc["wifiConfigs"] = this->memory->getWifiConfigs();
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

void Brain::addWifiConfig(){
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

    doc["message"] = "Wifi config added";
    doc["date"] = getDateTime();
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::deleteAllWifiConfig(){
    String messageJson;
    StaticJsonDocument<512> doc;
    errorContainer errorCont;
    this->memory->deleteAllWifiConfig();

    doc["message"] = "Wifi configs deleted";
    doc["date"] = getDateTime();
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::deleteWifiConfig(){
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
    this->memory->deleteWifiConfig(ssid);

    doc["message"] = "Wifi config deleted";
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

void Brain::getVoice(){
    String messageJson;
    StaticJsonDocument<512> doc;
    doc["date"] = getDateTime();
    doc["voice"]["isMute"] = voice->isMute;
    doc["voice"]["buzzerPin"] = BUZZER_PIN;
    doc["voice"]["voiceTypes"][0] = "alarm";
    doc["voice"]["voiceTypes"][1] = "emergency";
    doc["voice"]["voiceTypes"][2] = "notification";
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::setVoice(){
    errorContainer errorCont;
    String postBody = brainServer->arg("plain");
    String messageJson;
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Voice] deserializeJson() failed: "));
        errorCont.date = getDateTime();
        errorCont.error = "Voice update failed";
        errorCont.errorType = "Voice";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    try {
        JsonObject postObj = postBodyDoc.as<JsonObject>();
        voice->isMute = postObj["isMute"];
    } catch (const std::exception& e){
        errorCont.date = getDateTime();
        errorCont.error = "Voice update failed";
        errorCont.errorType = "Voice";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    doc["date"] = getDateTime();
    doc["message"] = "Voice updated";
    doc["voice"]["isMute"] = voice->isMute;
    doc["voice"]["buzzerPin"] = BUZZER_PIN;
    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::makeSound(){
    String messageJson;
    errorContainer errorCont;
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    String postBody = brainServer->arg("plain");
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Voice] deserializeJson() failed: "));
        errorCont.date = getDateTime();
        errorCont.error = "Voice update failed";
        errorCont.errorType = "Voice";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    try {
        JsonObject postObj = postBodyDoc.as<JsonObject>();
        String voiceType = postObj["voiceType"];

        if(voiceType == "alarm"){
            voice->alarm();
        }
        else if(voiceType == "emergency"){
            voice->emergency();
        }
        else if(voiceType == "notification"){
            voice->notification();
        }
        else{
            errorCont.date = getDateTime();
            errorCont.error = "Voice update failed";
            errorCont.errorType = "Voice";
            errorCont.errorCode = 400;
            returnError(errorCont);
            return;
        }
    } catch (const std::exception& e){
        errorCont.date = getDateTime();
        errorCont.error = "Voice update failed";
        errorCont.errorType = "Voice";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    doc["date"] = getDateTime();
    doc["message"] = "Sound played";
    doc["voice"]["isMute"] = voice->isMute;
    doc["voice"]["buzzerPin"] = BUZZER_PIN;
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

void Brain::getImageFromFile(){ // post method
    String messageJson;
    errorContainer errorCont;
    String postBody = brainServer->arg("plain");
    DynamicJsonDocument doc(1024*15);
    StaticJsonDocument<512> postBodyDoc;
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Image] deserializeJson() failed: "));
        errorCont.date = getDateTime();
        errorCont.error = "Image search failed";
        errorCont.errorType = "Image";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    try {
        JsonObject postObj = postBodyDoc.as<JsonObject>();
        String imagePath = postObj["imagePath"];
        String base64Image = this->memory->getImageFromPath(imagePath);

        doc["date"] = getDateTime();
        doc["image"]["data"] = base64Image;
        doc["image"]["path"] = imagePath;
        doc["image"]["status"] = true;
        doc["Eye"]["isFlashOnCapture"] = eye->isFlashOnCapture;
        doc["Eye"]["isFlashOn"] = eye->flashState;
        doc["Eye"]["autoSave"] = eye->autoSave;
        doc["Eye"]["flashPin"] = FLASH_PIN;
    } catch (const std::exception& e){
        errorCont.date = getDateTime();
        errorCont.error = "Image update failed";
        errorCont.errorType = "Image";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}

void Brain::deleteImage(){ // post method
    String messageJson;
    errorContainer errorCont;
    String postBody = brainServer->arg("plain");
    StaticJsonDocument<512> doc;
    StaticJsonDocument<512> postBodyDoc;
    DeserializationError error = deserializeJson(postBodyDoc, postBody);

    if (error) {
        Serial.print(F("[Image] deserializeJson() failed: "));
        errorCont.date = getDateTime();
        errorCont.error = "Image delete failed";
        errorCont.errorType = "Image";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    try {
        JsonObject postObj = postBodyDoc.as<JsonObject>();
        String imagePath = postObj["imagePath"];
        this->memory->deleteImage(imagePath);

        doc["date"] = getDateTime();
        doc["image"]["path"] = imagePath;
    } catch (const std::exception& e){
        errorCont.date = getDateTime();
        errorCont.error = "Image delete failed";
        errorCont.errorType = "Image";
        errorCont.errorCode = 400;
        returnError(errorCont);
        return;
    }

    serializeJson(doc, messageJson);
    brainServer->send(200, "application/json", messageJson);
}







