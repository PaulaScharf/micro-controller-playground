#include "Wire.h"
#include <SD.h>
#include "SPI.h"
#define VSPI_MISO   13
#define VSPI_MOSI   11
#define VSPI_SCLK   12
#define VSPI_SS     10
#define SD_ENABLE   9

#include "Freenove_WS2812_Lib_for_ESP32.h"
#define LED_PIN 1
Freenove_ESP32_WS2812 led = Freenove_ESP32_WS2812(1, LED_PIN, 0, TYPE_GRB);
// This sketch code is based on the RPLIDAR driver library provided by RoboPeak
#include <RPLidar.h>
// You need to create an driver instance
RPLidar lidar;

#define RPLIDAR_MOTOR 3 // The PWM pin for control the speed of RPLIDAR's motor.

int start = 0;
long measurementStartTime = 0;
String dataStr = "";
char buffer[7];
void setLED(uint8_t r,uint8_t g,uint8_t b) {
  led.setLedColorData(0, r, g, b);
  led.show();
}
void receiveEvent(int bytes) {
  Serial.print("received: ");
  start = Wire.read();    // read one character from the I2C
  Serial.println(start);
}

float minDistance = 100000;
float angleAtMinDist = 0;
int nsbit = 0;
float prevAngle = 360.0;
float lastRotationTime = millis();
File file;
int counter = 3;
void setup() {
  Serial.begin(921600);
  // while (!Serial) ;
  delay(2000);
  led.begin();
  led.setBrightness(30);
  setLED(60,60,0);
  Serial.println("hello");

  measurementStartTime = millis();
  setLED(0,0,60);
  Serial1.begin(115200,SERIAL_8N1, RX, TX);
  Serial.println("starting Lidar...");
  lidar.begin(Serial1);

  // set pin modes
  pinMode(RPLIDAR_MOTOR, OUTPUT);
  digitalWrite(RPLIDAR_MOTOR,HIGH);
  Serial.println("Opened file for appending");
  while(1==1) {
    analogWrite(RPLIDAR_MOTOR, analogRead(A0));
    if (IS_OK(lidar.waitPoint())) {
      // dataStr = "";
      //convert floats to string and assemble c-type char string for writing:
      dataStr += String(millis()-measurementStartTime) + ", ";//add it onto the end
      //perform data processing here...
      float distance = lidar.getCurrentPoint().distance;
      float angle = lidar.getCurrentPoint().angle;
      dataStr += String(angle) + ", ";//add it onto the end
      dataStr += String(distance) + ", ";//add it onto the end
      dataStr += ";";
      if(prevAngle>angle+10) {
        counter--;
        if(counter<=0) {
        Serial.println(dataStr);
        dataStr = "";
        counter = 3;
        }
      };
      prevAngle = angle;
    } else {
      analogWrite(RPLIDAR_MOTOR, 0); //stop the rplidar motor

      // try to detect RPLIDAR...
      rplidar_response_device_info_t info;
      if (IS_OK(lidar.getDeviceInfo(info, 100))) {
        //detected...
        lidar.startScan();
        // analogWrite(RPLIDAR_MOTOR, 255);
        analogWrite(RPLIDAR_MOTOR, analogRead(A0));
        delay(1000);
      }
    }
  }
  setLED(60,0,0);

  digitalWrite(RPLIDAR_MOTOR,0);
  file.close();
  setLED(0,60,0);
}
void loop() {
  delay(100000);
}