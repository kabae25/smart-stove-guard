#pragma once
#include "Arduino.h"

#define MOTION_PIN 1

class BS412 {
    private:

    public:
        void init(void);
        bool getState(void);
};

void BS412::init(void) {
    Serial.println("Creating BS412");

    pinMode(MOTION_PIN, INPUT);

    Serial.println("BS412 Ready");
}

bool BS412::getState(void) {
    int state = digitalRead(MOTION_PIN);
    return bool(state);
}