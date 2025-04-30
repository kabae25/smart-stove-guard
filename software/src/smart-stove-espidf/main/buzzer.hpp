#pragma once

#include "Arduino.h"

#define BUZZER_PIN 13


class Buzzer {
    public:
        void init();
        void setActive(void);
        void setInactive(void);
};

void Buzzer::init(void) {
    Serial.println("Creating Buzzer");

    pinMode(BUZZER_PIN, OUTPUT);
    Serial.print("Pin: ");
    Serial.print(BUZZER_PIN);
    Serial.println(" OUTPUT");

    Serial.println("Buzzer Ready");
}

void Buzzer::setActive(void) {
    digitalWrite(BUZZER_PIN, 1);
}

void Buzzer::setInactive(void) {
    digitalWrite(BUZZER_PIN, 0);
}