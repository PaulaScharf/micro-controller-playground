#include "Wire.h"
#include <MPU6050_light.h>

#define I2C_PIN_SCL 42
#define I2C_PIN_SDA 45

MPU6050 mpu(Wire);

/**********************************

SENSORS

****************************************/
void getMPU6050Values(float arr[3]) {
  mpu.update();
  arr[0] = mpu.getAngleX(); // x
  arr[1] = mpu.getAngleY(); // y
  arr[2] = mpu.getAngleZ(); // z
}

/*************************

Sensor Setup

*************************/
void setupMPU6050() {
  Wire.begin(I2C_PIN_SDA, I2C_PIN_SCL);
  
  mpu.setAddress(0x68); // set I2C address
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true,true); // gyro and accelero
  Serial.println("Done!\n");
}
