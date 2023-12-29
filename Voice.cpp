//
// Created by Ata Can Yaymacı on 29.12.2023.
//

#include "Voice.h"

Voice::Voice() {
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(ALARM_BUTTON_PIN, INPUT);
}

void Voice::alarm() { // alarm until button pressed
    int alarmCount = 25;
    if(!this->isMute) {
        while(!digitalRead(ALARM_BUTTON_PIN) == HIGH && alarmCount > 0) {
            digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
            digitalWrite(BUZZER_PIN, LOW);
            delay(200);
            alarmCount--;
        }
    }
}

void Voice::emergency() { // alarm until button pressed
    if(!this->isMute) {
        for (int i = 0; i < 5; ++i) {
            digitalWrite(BUZZER_PIN, HIGH);
            delay(200);
            digitalWrite(BUZZER_PIN, LOW);
            delay(100);
        }
    }
}

void Voice::notification() { // alarm until button pressed
    if(!this->isMute) {
        for (int i = 0; i < 2; ++i) {
            digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
            digitalWrite(BUZZER_PIN, LOW);
            delay(100);
        }
    }
}
