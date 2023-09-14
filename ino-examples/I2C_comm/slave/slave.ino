#include "Wire.h"

#include "Freenove_WS2812_Lib_for_ESP32.h"
#define LED_PIN 1
Freenove_ESP32_WS2812 led = Freenove_ESP32_WS2812(1, LED_PIN, 0, TYPE_GRB);

int x = 0;
void setup() {
  Serial.begin(921600);
  Serial.println("setting up");
  led.begin();
  led.setBrightness(30);  
  // Start the I2C Bus as Slave on address 9
  Wire.begin(9,39,40,100000); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  Serial.println("finished setting up");
}
void setLED(uint8_t r,uint8_t g,uint8_t b) {
  led.setLedColorData(0, r, g, b);
  led.show();
}
void receiveEvent(int bytes) {
  x = Wire.read();    // read one character from the I2C
  Serial.println(x);
}
void loop() {
  //If value received is 0 blink LED for 200 ms
  if (x == 0) {
    setLED(0,0,60);
    delay(200);
    setLED(60,60,60);
    delay(200);
  }
  //If value received is 3 blink LED for 400 ms
  else if (x == 3) {
    setLED(0,60,0);
    delay(400);
    setLED(60,60,60);
    delay(400);
  } else {
    setLED(60,0,0);
    delay(100);
    setLED(60,60,60);
    delay(100);
  }
  Wire.write("hello");
}