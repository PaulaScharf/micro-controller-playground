/*
  Read an 8x8 array of distances from the VL53L5CX
  By: Nathan Seidle
  SparkFun Electronics
  Date: October 26, 2021
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example shows how to get all 64 pixels, at 15Hz, comma seperated output.
  This is handy for transmission to visualization programs such as Processing.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/18642
*/

#include <Wire.h>

#define I2C_PIN_SCL 40
#define I2C_PIN_SDA 39

#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 byes of RAM

String incomingText = ""; // for incoming serial data

int imageResolution = 0; // Used to pretty print output
int imageWidth = 0;      // Used to pretty print output

long measurements = 0;         // Used to calculate actual output rate
long measurementStartTime = 0; // Used to calculate actual output rate

bool readToF = false;
bool readSoil = false;

float getSMT50Temperature(int analogPin){
  int sensorValue = analogRead(analogPin);
  float voltage = sensorValue * (3.3 / 1024.0);
   return (voltage - 0.5) * 100;
}
float getSMT50Moisture(int analogPin){
   int sensorValue = analogRead(analogPin);
    float voltage = sensorValue * (3.3 / 1024.0);
   return (voltage * 50) / 3;
}
void printSoilSensor() {
  if(readToF) Serial.print("s");
  Serial.print(String(getSMT50Temperature(1)) + ",");
  Serial.print(String(getSMT50Moisture(2)));
  Serial.println();
}
void printToFImager() {
  if(readSoil) Serial.print("t");
  if (myImager.getRangingData(&measurementData)) // Read distance data into array
    {
      // The ST library returns the data transposed from zone mapping shown in datasheet
      // Pretty-print data with increasing y, decreasing x to reflect reality
      for (int y = 0; y <= imageWidth * (imageWidth - 1); y += imageWidth)
      {
        for (int x = imageWidth - 1; x >= 0; x--)
        {
          // Serial.print(measurementData.reflectance[x + y]);
          if(measurementData.target_status[x + y] == 255) {
            Serial.print(3000);
          } else {
            Serial.print(measurementData.distance_mm[x + y]);
          }
          Serial.print(",");
        }
      }
      Serial.println();

      // Uncomment to display actual measurement rate
      // measurements++;
      // float measurementTime = (millis() - measurementStartTime) / 1000.0;
      // Serial.print("rate: ");
      // Serial.print(measurements / measurementTime, 3);
      // Serial.println("Hz");
    }
}


void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("SparkFun VL53L5CX Imager Example");

  Wire.begin(I2C_PIN_SDA, I2C_PIN_SCL);
  // Wire.begin();
  Wire.setClock(1000000); //Sensor has max I2C freq of 1MHz

  // myImager.setWireMaxPacketSize(128); // Increase default from 32 bytes to 128 - not supported on all platforms

  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
  // myImager.setAddress(0x68);
  if (myImager.begin() == false)
  {
    while (1) {
    Serial.println(F("Sensor not found - check your wiring. Freezing"));
    delay(2000);
    };
  }

  myImager.setResolution(8 * 8); // Enable all 64 pads

  imageResolution = myImager.getResolution(); // Query sensor for current resolution - either 4x4 or 8x8
  imageWidth = sqrt(imageResolution);         // Calculate printing width

  // Using 4x4, min frequency is 1Hz and max is 60Hz
  // Using 8x8, min frequency is 1Hz and max is 15Hz
  myImager.setRangingFrequency(15);
  myImager.setRangingMode(SF_VL53L5CX_RANGING_MODE::CONTINUOUS);

  myImager.startRanging();

  measurementStartTime = millis();
}

void loop()
{
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingText = Serial.readString();
    if(incomingText.indexOf("s") >= 0) {
      readSoil = !readSoil;
    } if(incomingText.indexOf("t") >= 0) {
      readToF = !readToF;
    }
  }
  // Poll sensor for new data
  if (readToF && myImager.isDataReady() == true)
  {
    printToFImager();
  }

  if (readSoil) {
    printSoilSensor();
  }

  delay(5); // Small delay between polling
}

