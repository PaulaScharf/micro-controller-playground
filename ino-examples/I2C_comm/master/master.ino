#include "Wire.h"

// Include the required Wire library for I2C<br>#include 
int x = 0;
void setup() {
  // Start the I2C Bus as Master
  Wire.begin(39, 40, 100000);
}
void loop() {
  Serial.println(x);
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(x);              // sends x 
  Wire.endTransmission();    // stop transmitting
  x++; // Increment x
  if (x > 5) x = 0; // `reset x once it gets 6
  if(Wire.available()) {
    Serial.println(Wire.read());
  }
  delay(500);
}