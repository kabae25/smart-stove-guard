#pragma once

#include "Arduino.h"

// Subsystems
#include "smarthome.hpp"
#include "TempSensor.hpp"
#include "MotionSensor.hpp"
#include "Button.hpp"
#include "led.hpp"
#include "buzzer.hpp"

#include "BlynkSimpleEsp32.h"

class StateMachine {
    private:
        static float current_temp;
        float average_temp = 0;
        float new_average_temp = 0;

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
    public:
        void init(void);
        void update(void);

        static void periodicSendTemp(void);
};


void StateMachine::init(void) {
    Serial.println("Creating StateMachine");

    // Init Subsystems
    led.init();
    temp.init();
    ms.init();
    //sh.init();
    btn.init();
    bz.init();

    //timer.setInterval(10000L, StateMachine::periodicSendTemp());

    Serial.println("StateMachine Ready");
    delay(5*1000);
    led.setLow();
}

void StateMachine::update(void) {
    //sh.update(); // Run Blynk.run()

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

    if (current_state > SYS_IDLE) {
         //timer.run(); // We Should only periodically update the temperature on Blynk when something is happening on the stove
        Serial.println(current_state);
        delay(1000);
    }
}

void StateMachine::periodicSendTemp(void) {
    //Blynk.virtualWrite(STOVE_TEMP_VIRT, StateMachine::current_temp);
    Serial.println("Uploading Temp");
}

inline void StateMachine::handleIdle(void)
{
    // If we see the motion sensor go off, we should go into the "stove active, waiting" state
    if (btn.getState()) {  // SHOULD BE MS.getState()
        //sh.publishMotionActive();
        Serial.println("Motion Detected While In Idle State");
        current_state = SYS_ACTIVE_WAITING;
        Serial.println("Waiting for falling edge of motion sensor");
    }
}

inline void StateMachine::handlePairing(void)
{
    // Hang here if the system doesn't connect to wifi. 
    // Change LED color
}

inline void StateMachine::handleActiveWaiting(void)
{
    if (!ms.getState()) { // On the falling edge of the Motion Sensor, take the temperature
        current_state = SYS_TEMP_READING;
        Serial.println("Saw Falling edge");
    }
}

inline void StateMachine::handleTempReading(void)
{
    Serial.println("Reading Temperature");
    average_temp = temp.buildTempReading(60, false); // Take the temperature 60 times and print debug messages
    Serial.println("Entering Cooldown State");
    current_state = SYS_COOLDOWN; // Wait for the stove to cool down
}

inline void StateMachine::handleCooldown(void)
{
    Serial.println("Waiting for Stove to Cooldown");
    delay(1000); // Wait Some amount of time
    new_average_temp = temp.buildTempReading(60, true);

    if (new_average_temp < 50) { // Avoid already Cool false alarms
        Serial.println("False Alarm: Stove already Cool!");
        current_state = SYS_IDLE;
        return;
    }
    
    if (abs(new_average_temp - average_temp) <= 10) { // If temp has not decreased 10 degrees, alarm
        current_state = SYS_ALARMING;
        Serial.println("Temperature Delta Detected, Alarming!");
    } else { // Otherwise, go to idle state
        Serial.println("False Alarm: Stove Cooling Down");
        current_state = SYS_IDLE;
    }
}

inline void StateMachine::handleAlarming(void)
{
    bz.setActive();
    delay(1000);
    bz.setInactive();
    delay(2000);
    if (btn.getState()) {
        Serial.println("Cancel Detected! Entering Idle");
        current_state = SYS_IDLE;
        bz.setActive();
        delay(100);
        bz.setInactive();
        delay(2000);
    }
}
