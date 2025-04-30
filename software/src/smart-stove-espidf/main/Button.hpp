#pragma once

#include "Arduino.h"

#define USER_BUTTON_PIN 11

class Button {
    private:
    protected:
    public:
        void init(void);
        void update(void);
        bool getState(void);
};

void Button::init(void) {
    Serial.println("Creating Button");

    pinMode(USER_BUTTON_PIN, INPUT);

    Serial.print("Pin: ");
    Serial.print(USER_BUTTON_PIN);
    Serial.println(" INPUT.");

    Serial.println("Button Created");
}

bool Button::getState(void) {
    bool state = digitalRead(USER_BUTTON_PIN);
    return state;
}