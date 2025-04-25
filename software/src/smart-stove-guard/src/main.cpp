#include <Arduino.h>

Fsm fsm;

void setup() {
    fsm.init();
}

void loop() {
    fsm.loop();
}