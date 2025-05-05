#include <Wire.h>
// MLX90614
#define SDA_PIN 5
#define SCL_PIN 6
#define MLX_ADDR 0x5A
#define REG_OBJECT_TEMP 0x07

/*
 * Code for LMX90614
 */
void initMLX90614() { // Initialize the MLX90614
  Serial.print("Creating MLX90614");

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

  Serial.print("MLX90614 Ready");
}

// Ready MLX90614 repeat-start i2c
float readMLX90614(uint8_t reg) {
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
float getTemp(bool debug = true) {
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

/*
 * Code for BS412 Motion Sensor
 */
void initBS412() {
  Serial.print("Creating BS412");





  Serial.print("");
}




/*
 * Code for the User Pushbutton
 */


/*
 * Code for the Buzzer
 */


/*
 * Code for the Status LED 
 */

/*
 * Code for the ESP32-C6 WiFi Transmission
 */


void setup() {
  Serial.begin(115200);
  delay(100);
  initMLX90614();
}

void loop() {
  delay(1000);
  getTemp(true);
}

