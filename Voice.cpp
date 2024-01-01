//
// Created by Ata Can Yaymacı on 29.12.2023.
//

#include "Voice.h"

Voice::Voice() {
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(ALARM_BUTTON_PIN, INPUT);
    digitalWrite(BUZZER_PIN, LOW);
}

void Voice::alarm() { // alarm until button pressed
    int alarmCount = 30;
    if(!this->isMute) {
        while((!digitalRead(ALARM_BUTTON_PIN) == HIGH) && alarmCount > 0) {
            tone(BUZZER_PIN, 1000);
            delay(100);
            tone(BUZZER_PIN, 2000);
            delay(100);
            tone(BUZZER_PIN, 3000);
            delay(100);
            tone(BUZZER_PIN, 2000);
            delay(100);
            tone(BUZZER_PIN, 1000);
            delay(100);
            noTone(BUZZER_PIN);
            delay(500);
            alarmCount--;
        }
    }
}

void Voice::emergency() { // alarm until button pressed
    if(!this->isMute) {
        for (int i = 0; i < 7; ++i) {
            tone(BUZZER_PIN, 500);
            delay(600);
            noTone(BUZZER_PIN);
            delay(200);
        }
    }
}

void Voice::notification() { // alarm until button pressed
    if(!this->isMute) {
        for (int i = 0; i < 2; ++i) {
            tone(BUZZER_PIN, 1000);
            delay(300);
            noTone(BUZZER_PIN);
            delay(200);
        }
    }
}
