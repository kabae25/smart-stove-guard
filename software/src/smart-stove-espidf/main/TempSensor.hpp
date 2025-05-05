#pragma once
#include "Arduino.h"
#include "Wire.h"

#define SDA_PIN 5
#define SCL_PIN 6
#define MLX_ADDR 0x5A
#define REG_OBJECT_TEMP 0x07

#define EMA_MULTIPLIER 0.7

class TempSensor {
    private:
      float readMLX90614(uint8_t reg);
    public:
      void init();
      float getTemp(bool debug);
      float buildTempReading(int num_iters, bool debug);

      void debug(void);
};

void TempSensor::init() { // Initialize the MLX90614
    Serial.println("Creating MLX90614");
  
    // Seting up I2C
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);
    delay(250);  // Let MLX90614 boot
  
    Serial.println("Scanning I2C...");
  
    for (uint8_t addr = 1; addr < 127; addr++) {
      Wire.beginTransmission(addr);
      if (Wire.endTransmission() == 0) {
        Serial.print("Found device at 0x");
        Serial.println(addr, HEX);
      }
    }
  
    Serial.println("MLX90614 Ready");
  }
  
  // Ready MLX90614 repeat-start i2c
  float TempSensor::readMLX90614(uint8_t reg) {
    Wire.beginTransmission(MLX_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false); // Repeated start
    Wire.requestFrom(MLX_ADDR, (uint8_t)3);
  
    if (Wire.available() < 3) return NAN;
  
    uint8_t lsb = Wire.read();
    uint8_t msb = Wire.read();
    Wire.read(); // PEC byte
  
    uint16_t raw = ((uint16_t)msb << 8) | lsb;
    return (raw * 0.02) - 273.15; // Celsius conversion
  }
  
  // Getter for Temperature
  float TempSensor::getTemp(bool debug = true) {
    float temp = readMLX90614(REG_OBJECT_TEMP);
    if (isnan(temp)) {
      Serial.println("Failed to read temperature.");
      return -1;
    } else {
      if (debug) {
        Serial.print("Object Temp: ");
        Serial.print(temp);
        Serial.println(" C");
      }
      return temp;
    }
  }

  float TempSensor::buildTempReading(int num_iters = 60, bool debug = true) {
    float ema_temp = getTemp(debug);
    int sample_cnt = 0;
    while (sample_cnt < num_iters) {
      ema_temp = (getTemp(debug) * EMA_MULTIPLIER) + (ema_temp * (1 - EMA_MULTIPLIER)); // Build exponential moving average
      sample_cnt++;
      delay(100); // Give the MLX90614 a little time to breath
    }

    if (debug) {
      Serial.print("Finished Building EMA: ");
      Serial.println(ema_temp);
    }
    return ema_temp;
  }

  inline void TempSensor::debug(void)
  {
    float temp = getTemp(true);
    Serial.print("Temp: ");
    Serial.println(temp);
    delay(100);
  }
