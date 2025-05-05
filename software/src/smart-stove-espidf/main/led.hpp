#pragma once
#include "Arduino.h"

#define LED_PIN 2

class LED {
    private:

    protected:

    public:
        void init(void);
        void setHigh(void);
        void setLow(void);
};

void LED::init(void) {
    Serial.println("Creating LED");
    pinMode(LED_PIN, OUTPUT);
    Serial.print("Set OUTPUT HIGH on Pin: ");
    Serial.println(LED_PIN);
    setHigh();
    Serial.println("LED Ready");
}

void LED::setHigh(void) {
    digitalWrite(LED_PIN, HIGH);
}

void LED::setLow(void) {
    digitalWrite(LED_PIN, LOW);
}

