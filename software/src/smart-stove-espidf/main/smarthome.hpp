#pragma once
#include "BlynkSimpleEsp32.h"
#include "WiFi.h"

#include "auth.h"

#define STOVE_TEMP_VIRT 0


class SmartHome {
    public:
        void init(void);
        void update(void);

        void publishMotionActive(void);
};

void SmartHome::init(void) {
    Blynk.begin(auth, ssid, pass);

    bool result = Blynk.connect();
    if (!result) {
      while (true) {
        Serial.println("Error: Blynk not connected. Please Fix.");
        delay(1000);
      }
    }
}

void SmartHome::update(void) {
    Blynk.run();
}

// Log a Blynk Event that motion has been detected - Stove is in use!
void SmartHome::publishMotionActive(void) {

}
