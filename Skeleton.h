#ifndef CYCLOPS_V2_0_SKELETON_H
#define CYCLOPS_V2_0_SKELETON_H

#include <Arduino.h>
#include <ESP32Servo.h>

#define HORIZONTAL_PIN 12
#define VERTICAL_PIN 13

class Skeleton { // For 2-axis pan-tilt
    public:
      int horizontalAxisAngle = 0; // X
      int verticalAxisAngle = 0; // Y
      Servo horizontalAxis;
      Servo verticalAxis;

      Skeleton();
      void setAxis(char axis, int angle);
      int addAxis(char axis, int addAngle);

    private:

      void calibrate();
      void setHorizontalAngle(int angle);
      void setVerticalAngle(int angle);
};

#endif //CYCLOPS_V2_0_SKELETON_H
