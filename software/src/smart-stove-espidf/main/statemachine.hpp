#pragma once

#include "Arduino.h"

// Subsystems
#include "smarthome.hpp"
#include "TempSensor.hpp"
#include "MotionSensor.hpp"
#include "Button.hpp"
#include "led.hpp"
#include "buzzer.hpp"

#include "auth.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "BlynkSimpleEsp32.h"

class StateMachine {
    private:
        static float current_temp;
        float average_temp = 0;
        float new_average_temp = 0;
        bool send_msg = false;
        unsigned long cooldownStart = 0;
        bool inCooldown = false;
        unsigned long long int timeoutcheck = 0;

    protected:
        // Hardware Subsystems
        TempSensor temp; 
        MotionSensor ms;
        SmartHome sh;
        Button btn;
        LED led;
        Buzzer bz;

        BlynkTimer timer; // For Periodic Temperature Checks

        enum SYS_STATE {
            SYS_IDLE,
            SYS_PAIRING,
            SYS_ACTIVE_WAITING,
            SYS_TEMP_READING,
            SYS_COOLDOWN,
            SYS_ALARMING
        };
        SYS_STATE current_state = SYS_IDLE;
        
        void handleIdle(void);
        void handlePairing(void);
        void handleActiveWaiting(void);
        void handleTempReading(void);
        void handleCooldown(void);
        void handleAlarming(void);
        void periodicTemperature(void);
        void finishCooldownCheck(void);
    public:
        void init(void);
        void update(void);
        bool getSendMsg(void) {return send_msg;}
        float getTempQuick(void) {return temp.buildTempReading(10, false);}
};

void StateMachine::init(void) {
    Serial.println("Creating StateMachine");

    // Init Subsystems
    led.init();
    temp.init();
    ms.init();
    btn.init();
    bz.init();

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    timer.setInterval(20000, [this](){ // little lambda wizardry
        this->periodicTemperature();
    });
    Blynk.virtualWrite(V1, "Idle: Waiting for motion...");

    delay(2*1000);
    Serial.println("StateMachine Ready");
    led.setLow();
}

void StateMachine::update(void) {
    Blynk.run();

    // If device disconnects, notify user
    if (!Blynk.connected()) {
        current_state = SYS_PAIRING;
    }

    // Run state Machine
    switch(current_state) {
        case(SYS_IDLE):
            handleIdle();
            break;
        case(SYS_PAIRING):
            handlePairing();
            break;
        case(SYS_ACTIVE_WAITING):
            handleActiveWaiting();
            break;
        case(SYS_TEMP_READING):
            handleTempReading();
            break;
        case(SYS_COOLDOWN):
            handleCooldown();
            break;
        case(SYS_ALARMING):
            handleAlarming();
            break;
    }

    if (current_state == SYS_COOLDOWN || current_state == SYS_ACTIVE_WAITING) {
        //Serial.println(current_state);
        send_msg = true;
    }
    else {
        send_msg = false;
    }

    timer.run(); // for blynk timers
    delay(10); // Let things chill out
}

inline void StateMachine::handleIdle(void)
{
    // If we see the motion sensor go off, we should go into the "stove active, waiting" state
    if (ms.getState()) {  // SHOULD BE MS.getState()
        Serial.println("Motion Detected While In Idle State, waiting for falling edge of sensor");
        Blynk.logEvent("stove_in_use");
        Blynk.virtualWrite(V1, "Motion Detected!");
        current_state = SYS_ACTIVE_WAITING;
        timeoutcheck = millis() + 1*30*1000;
    }
}

inline void StateMachine::handlePairing(void)
{
    led.setHigh();
    if (Blynk.connected()) {
        current_state = SYS_IDLE; 
        led.setLow();
    }
}

inline void StateMachine::handleActiveWaiting(void)
{
    if (!ms.getState()) { // On the falling edge of the Motion Sensor, take the temperature
        current_state = SYS_TEMP_READING;
        led.setHigh();
        delay(100);
        led.setLow();
        Serial.println("Saw Falling edge");
    } 
    if ((timeoutcheck - millis()) < 1000) {
        Serial.println("Timeout exceeded, going to keep going for demo");
        led.setHigh();
        delay(100);
        led.setLow();
        current_state = SYS_TEMP_READING;
    }
}

inline void StateMachine::handleTempReading(void)
{
    Serial.println("Reading Temperature");
    average_temp = temp.buildTempReading(60, true); // Take the temperature 60 times and print debug messages
    Serial.println("Entering Cooldown State");

    if (average_temp <= 30) {
        Blynk.virtualWrite(V1, "Idle: Waiting for motion...");
        current_state = SYS_IDLE;
        return;
    }

    Blynk.virtualWrite(V1, "Stove is hot! Waiting 2 min for cooldown");
    current_state = SYS_COOLDOWN; // Wait for the stove to cool down
}

inline void StateMachine::handleCooldown() {
    if (!inCooldown) {
      // 1) First entry into cooldown:
      Serial.println("Waiting 1 minutes for Stove to Cooldown");
      cooldownStart = millis();
      inCooldown    = true;
      return;
    }
  
    // 2) Still counting down?
    if (millis() - cooldownStart < 2UL*60*1000) {
      // not yet expired — return immediately (no blocking)
      return;
    }
  
    // 3) Timer expired, do the check:
    inCooldown = false;
    finishCooldownCheck();
}

void StateMachine::finishCooldownCheck() {
    new_average_temp = temp.buildTempReading(60, true);
      
    if ((new_average_temp - average_temp) > -2) {
      Serial.println("Temperature Delta Detected, Alarming!");
      Blynk.virtualWrite(V1, "ALERT! STOVE LEFT ON!");
      current_state = SYS_ALARMING;
      Blynk.logEvent("stove_alert");
    } 
    else {
      Blynk.virtualWrite(V1, "Idle: Waiting for motion...");
      Serial.println("Stove cooled down.");
      current_state = SYS_IDLE;
    }
}

inline void StateMachine::handleAlarming(void)
{
    bz.setActive();
    led.setHigh();
    delay(1000);
    bz.setInactive();
    led.setLow();
    delay(500);
    if (btn.getState()) {
        Serial.println("Cancel Detected! Entering Idle");
        Blynk.virtualWrite(V1, "Idle: Waiting for motion...");
        current_state = SYS_IDLE;
        bz.setActive();
        led.setHigh();
        delay(100);
        led.setLow();
        bz.setInactive();
        delay(2000);
    }
}

/**
 * For Blynk Functionality
 */
void StateMachine::periodicTemperature(void) {
    if (getSendMsg()) {
        float temp = getTempQuick();
        Blynk.virtualWrite(V0, temp);
        Serial.println("Send Periodic Temperature Update");
    }
}