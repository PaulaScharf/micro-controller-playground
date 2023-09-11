#include "Freenove_WS2812_Lib_for_ESP32.h"
#define LED_PIN 1

// This sketch code is based on the RPLIDAR driver library provided by RoboPeak
#include <RPLidar.h>
// You need to create an driver instance
RPLidar lidar;

#define RPLIDAR_MOTOR 3 // The PWM pin for control the speed of RPLIDAR's motor.

Freenove_ESP32_WS2812 led = Freenove_ESP32_WS2812(1, LED_PIN, 0, TYPE_GRB);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(921600);
  while (!Serial) ;

  Serial1.begin(115200,SERIAL_8N1, RX, TX);
  Serial.println("starting Lidar...");
  lidar.begin(Serial1);

  // set pin modes
  pinMode(RPLIDAR_MOTOR, OUTPUT);
  digitalWrite(RPLIDAR_MOTOR,HIGH);

  led.begin();
  led.setBrightness(30);  
  setLED(0,255,0);
}

void setLED(uint8_t r,uint8_t g,uint8_t b) {
  led.setLedColorData(0, r, g, b);
  led.show();
}

float minDistance = 100000;
float angleAtMinDist = 0;
int nsbit = 0;
float prevAngle = 360.0;
float lastRotationTime = millis();
void loop() {
  Serial.println(millis());
  // analogWrite(RPLIDAR_MOTOR, analogRead(A0));
  // if (IS_OK(lidar.waitPoint())) {
  //   //perform data processing here...
  //   float distance = lidar.getCurrentPoint().distance;
  //   float angle = lidar.getCurrentPoint().angle;
  //   // Serial.print(distance);
  //   if(angle<prevAngle-10) {
  //     // Serial.print(".");
  //     // Serial.print(angle);
  //     // Serial.print(" ");
  //     // Serial.print(prevAngle);
  //     // Serial.print(" ");
  //     // Serial.println(millis()-lastRotationTime);
  //     // lastRotationTime = millis();
  //     Serial.println(distance);
  //   } else {
  //     // Serial.print("-");
  //     // Serial.print(" ");
  //     // Serial.print(millis()-lastRotationTime);
  //     // Serial.print(" ");
  //     // lastRotationTime = millis();
  //   }
  //   prevAngle = angle;
  // } else {
  //   analogWrite(RPLIDAR_MOTOR, 0); //stop the rplidar motor

  //   // try to detect RPLIDAR...
  //   rplidar_response_device_info_t info;
  //   if (IS_OK(lidar.getDeviceInfo(info, 100))) {
  //     //detected...
  //     lidar.startScan();
  //     // analogWrite(RPLIDAR_MOTOR, 255);
  //     analogWrite(RPLIDAR_MOTOR, analogRead(A0));
  //     delay(1000);
  //   }
  // }
}