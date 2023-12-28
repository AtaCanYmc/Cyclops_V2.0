#include "Skeleton.h"

Skeleton::Skeleton(){
    pinMode(HORIZONTAL_PIN, OUTPUT);
    pinMode(VERTICAL_PIN, OUTPUT);
    horizontalAxis.attach(HORIZONTAL_PIN);
    verticalAxis.attach(VERTICAL_PIN);
    this->calibrate();
}

void Skeleton::setAxis(char axis='Y', int angle=0)
{
    if(axis == 'X')
    {
        setHorizontalAngle(angle);
    }
    else if(axis == 'Y')
    {
        setVerticalAngle(angle);
    }
}

int Skeleton::addAxis(char axis='Y', int addAngle=10){
    if(axis == 'X')
    {
        setHorizontalAngle(horizontalAxisAngle + addAngle);
    }
    else if(axis == 'Y')
    {
        setVerticalAngle(verticalAxisAngle + addAngle);
    }
}

void Skeleton::calibrate(){
    setHorizontalAngle(0);
    setVerticalAngle(0);
}

void Skeleton::setHorizontalAngle(int angle=0)
{
    if(angle > 180)
    {
        angle = 180;
    }
    else if(angle < -180)
    {
        angle = -180;
    }
    horizontalAxisAngle = angle;
    this->horizontalAxis.write(angle);
}

void Skeleton::setVerticalAngle(int angle=0)
{
    if(angle > 180)
    {
        angle = 180;
    }
    else if(angle < -180)
    {
        angle = -180;
    }
    verticalAxisAngle = angle;
    this->verticalAxis.write(angle);
}



