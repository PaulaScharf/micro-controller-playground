/*
  IMU Capture
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU and prints it to the Serial Monitor for one second
  when the significant motion is detected.
  You can also use the Serial Plotter to graph the data.
  The circuit:
  - Arduino Nano 33 BLE or Arduino Nano 33 BLE Sense board.
  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry
  This example code is in the public domain.
*/

#include "Wire.h"
#include <MPU6050_light.h>

#define I2C_PIN_SCL 42
#define I2C_PIN_SDA 45

MPU6050 mpu(Wire);

#include "Freenove_WS2812_Lib_for_ESP32.h"
#define LED_PIN 1
Freenove_ESP32_WS2812 led = Freenove_ESP32_WS2812(1, LED_PIN, 0, TYPE_GRB);

float accelerationThreshold = 0.1; // threshold of significant in G's
const int numSamples = 119;

int samplesRead = numSamples;


float aX, aY, aZ, gX, gY, gZ;
const int numCaliSamples=2000;
float aXdef, aYdef, aZdef = 1000.0;


void setLED(uint8_t r,uint8_t g,uint8_t b) {
  led.setLedColorData(0, r, g, b);
  led.show();
}

void setup() {
  led.begin();
  led.setBrightness(20);  
  setLED(100,100,100); // green

  Serial.begin(115200);
  Wire.begin(I2C_PIN_SDA, I2C_PIN_SCL);
  
  while(!Serial);
  mpu.setAddress(0x68); // set I2C address
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){
    setLED(255,0,0); // red
    delay(10000);
  } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true,true); // gyro and accelero
  Serial.println("Done!\n");

  setLED(100,0,100); // green
  Serial.println(F("Calculating default acceleration"));
  for(int i = 0; i < numCaliSamples; i++) {
    aX = fabs(mpu.getAccX());
    aY = fabs(mpu.getAccY());
    aZ = fabs(mpu.getAccZ());
    if(aXdef>aX) {
      aXdef=aX;
    }
    if(aYdef>aY) {
      aYdef=aY;
    }
    if(aZdef>aZ) {
      aZdef=aZ;
    }
  }
  accelerationThreshold = aXdef + aYdef + aZdef;
  Serial.print(F("Acceleration threshold: "));
  Serial.println(accelerationThreshold);

  // print the header
  Serial.println("aX,aY,aZ,gX,gY,gZ");
  setLED(0,100,0); // green
}

void loop() {

  // wait for significant motion
  while (samplesRead == numSamples) {
    mpu.update();
    aX = mpu.getAccX();
    aY = mpu.getAccY();
    aZ = mpu.getAccZ();

    // sum up the absolutes
    float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

    // check if it's above the threshold
    if (aSum >= accelerationThreshold) {

      setLED(0,0,100); // green
      Serial.print(aX, 3);
      Serial.print(',');
      Serial.print(aY, 3);
      Serial.print(',');
      Serial.print(aZ, 3);
      Serial.print(',');
      Serial.print(gX, 3);
      Serial.print(',');
      Serial.print(gY, 3);
      Serial.print(',');
      Serial.print(gZ, 3);
      Serial.println();
      // reset the sample read count
      samplesRead = 0;
      break;
    }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    mpu.update();
    // check if both new acceleration and gyroscope data is
    // available
      // read the acceleration and gyroscope data
    aX = mpu.getAccX();
    aY = mpu.getAccY();
    aZ = mpu.getAccZ();
    gX = mpu.getGyroX();
    gY = mpu.getGyroY();
    gZ = mpu.getGyroZ();

    samplesRead++;

    // print the data in CSV format
    Serial.print(aX, 3);
    Serial.print(',');
    Serial.print(aY, 3);
    Serial.print(',');
    Serial.print(aZ, 3);
    Serial.print(',');
    Serial.print(gX, 3);
    Serial.print(',');
    Serial.print(gY, 3);
    Serial.print(',');
    Serial.print(gZ, 3);
    Serial.println();

    if (samplesRead == numSamples) {
      // add an empty line if it's the last sample
      Serial.println();

      setLED(0,100,0); // green
    }
  }
}