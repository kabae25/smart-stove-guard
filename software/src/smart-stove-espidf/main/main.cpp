#include "Arduino.h"
#include "mlx90614.hpp"
#include "bs412.hpp"

extern "C" void app_main()
{
    initArduino();

    Serial.begin(115200);
    while(!Serial){
        // Wait for Serial port to connect
    }
    // Make the sensor Modules
    MLX90614 mlx; 
    BS412 bs;
    
    mlx.init();
    bs.init();

    uint8_t sample_cnt = 0;
    float average_temp = 0;
    float new_average_temp = 0;
    // Connect to WiFi Here

    while(true) {
        mlx.getTemp(true);
        delay(1000);
        // Do Nothing
        if (bs.getState()) { // If I detect Motion:
          if (!bs.getState()) { // Wait for falling edge of the motion sense
            average_temp = mlx.buildTempReading(60, true);
            
            delay(2*60*1000); // Wait 2 Minutes
            
            new_average_temp = mlx.buildTempReading(60, true);

            if (abs(new_average_temp - average_temp) <= 10) { // if the temp hasn't changed more than 10 degrees celcius
              // freak out
            }
            // otherwise do nothing
          }
        }
    }
}

