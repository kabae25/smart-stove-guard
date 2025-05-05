#define BLYNK_TEMPLATE_ID "TMPL2kSeR0OuC"
#define BLYNK_TEMPLATE_NAME "Heat Herald"

// Needs to Be global for the periodicTimerFunction
#include "Arduino.h"

#include "statemachine.hpp"

extern "C" void app_main()
{
    initArduino();

    StateMachine stateMachine;

    Serial.begin(115200);
    while(!Serial){
        // Wait for Serial port to connect
    }

    stateMachine.init();

    while(true) {
      stateMachine.update();

      delay(100);
    }
}


