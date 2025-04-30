#pragma once

#include "Arduino.h"

// Subsystems
#include "smarthome.hpp"
#include "TempSensor.hpp"
#include "MotionSensor.hpp"
#include "Button.hpp"
#include "led.hpp"

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
    temp.init();
    ms.init();
    sh.init();
    btn.init();
    led.init();

    timer.setInterval(10000L, StateMachine::periodicSendTemp());

    Serial.println("StateMachine Ready");
}

void StateMachine::update(void) {
    sh.update(); // Run Blynk.run()

    switch(current_state) {
        case(SYS_IDLE):
            handleIdle();
        case(SYS_PAIRING):
            handlePairing();
        case(SYS_ACTIVE_WAITING):
            handleActiveWaiting();
        case(SYS_TEMP_READING):
            handleTempReading();
        case(SYS_COOLDOWN):
            handleCooldown();
        case(SYS_ALARMING):
            handleAlarming();
    }

    if (current_state >= SYS_ACTIVE_WAITING) {
        timer.run(); // We Should only periodically update the temperature on Blynk when something is happening on the stove
    }
}

void StateMachine::periodicSendTemp(void) {
    Blynk.virtualWrite(STOVE_TEMP_VIRT, StateMachine::current_temp);
    Serial.println("Uploading Temp");
}

inline void StateMachine::handleIdle(void)
{

    // If we see the motion sensor go off, we should go into the "stove active, waiting" state
    if (ms.getState()) { 
        sh.publishMotionActive();
        current_state = SYS_ACTIVE_WAITING;
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
    }
}

inline void StateMachine::handleTempReading(void)
{
    average_temp = temp.buildTempReading(60, true); // Take the temperature 60 times and print debug messages

    current_state = SYS_COOLDOWN; // Wait for the stove to cool down
}

inline void StateMachine::handleCooldown(void)
{
    delay(2*60*1000); // Wait Some amount of time
    new_average_temp = temp.buildTempReading(60, true);

    if (new_average_temp < 50) { // Avoid already Cool false alarms
        Serial.println("False Alarm");
        current_state = SYS_IDLE;
        return;
    }
    
    if (abs(new_average_temp - average_temp) <= 10) { // If temp has not decreased 10 degrees, alarm
        current_state = SYS_ALARMING;
    } else { // Otherwise, go to idle state
        current_state = SYS_IDLE;
    }
}

inline void StateMachine::handleAlarming(void)
{

}
