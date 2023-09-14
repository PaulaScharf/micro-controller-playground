#include "Wire.h"

// Include the required Wire library for I2C<br>#include 
void setup() {
  Serial.begin(921600);
  while (!Serial) ;
  // Start the I2C Bus as Master
  Wire.begin(39, 40, 100000);
  for(int i = 3; i>0; i--) {
    Serial.println(i);
    delay(500);
  }
  Serial.println("sending");
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(1);              // sends x 
  Wire.endTransmission();    // stop transmitting
  Wire.begin(39, 40, 100000);
  for(int i = 600; i>0; i--) {
    Serial.println(i);
    delay(1000);
  }
  Serial.println("sending");
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(2);              // sends x 
  Wire.endTransmission();    // stop transmitting
}
void loop() {
  delay(500);
}