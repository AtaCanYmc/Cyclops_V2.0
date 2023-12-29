//
// Created by Ata Can Yaymacı on 29.12.2023.
//

#ifndef CYCLOPS_V2_0_VOICE_H
#define CYCLOPS_V2_0_VOICE_H
#include <Arduino.h>

#define BUZZER_PIN 14
#define ALARM_BUTTON_PIN 15

class Voice {
public:
    bool isMute = false;
    Voice();
    void alarm();
    void emergency();
    void notification();
};


#endif //CYCLOPS_V2_0_VOICE_H
