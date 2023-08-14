#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 byes of RAM

#define TEMP_IO 3
#define HUMI_IO 2
#define IO_ENABLE 8

int imageResolution = 0; // Used to pretty print output
int imageWidth = 0;      // Used to pretty print output

long measurements = 0;         // Used to calculate actual output rate
long measurementStartTime = 0; // Used to calculate actual output rate

/**********************************

SENSORS

****************************************/
float getSMT50Temperature(int analogPin){
  int sensorValue = analogRead(analogPin);
  float voltage = sensorValue * (3.3 / 8190.0);
  return (voltage - 0.5) * 100;
}
float getSMT50Moisture(int analogPin){
  int sensorValue = analogRead(analogPin);
  float voltage = sensorValue * (3.3 / 8190.0);
  return (voltage * 50) / 3;
}
void getSoilSensorValues(int arr[2]) {
  arr[0] = getSMT50Temperature(TEMP_IO);
  arr[1] = getSMT50Moisture(HUMI_IO);
}
void getToFValues(int arr[3]) {
  arr[0] = -1; // max
  arr[1] = -1; // min
  arr[2] = -1; // no reading
  if (myImager.isDataReady() == true)
  {
    // char result[64];
    // strcpy(result,"");
    arr[0] = 0;
    arr[1] = 5000;
    arr[2] = 0;
    // if(readSoil) result += "t";
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
            arr[2] += 1;
          } else {
            int measurement = measurementData.distance_mm[x + y];
            if (measurement>arr[0]) {
              arr[0] = measurement;
            } if (measurement<arr[1]) {
              arr[1] = measurement;
            }
          }
        }
        }
      // Serial.println(String(min) + "," + String(max) + "," + String(noReading));

      // Uncomment to display actual measurement rate
      // measurements++;
      // float measurementTime = (millis() - measurementStartTime) / 1000.0;
      // Serial.print("rate: ");
      // Serial.print(measurements / measurementTime, 3);
      // Serial.println("Hz");
    }
  }
}

/*************************

Sensor Setup

*************************/
// plug into any of the two I2C interfaces
void setupToFImager() {
  // I2C for Imager
  Wire.begin(I2C_PIN_SDA, I2C_PIN_SCL);
  Wire.setClock(1000000); //Sensor has max I2C freq of 1MHz
  Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
  // myImager.setAddress(0x68);
  if (myImager.begin() == false)
  {
    while (1) {
    Serial.println(F("Sensor not found - check your wiring. Freezing"));
    delay(2000);
    };
  }
  myImager.setResolution(4*4); // Enable all 64 pads
  imageResolution = myImager.getResolution(); // Query sensor for current resolution - either 4x4 or 8x8
  imageWidth = sqrt(imageResolution);         // Calculate printing width
  myImager.setRangingFrequency(15);
  myImager.setRangingMode(SF_VL53L5CX_RANGING_MODE::CONTINUOUS);

  myImager.startRanging();
  
  measurementStartTime = millis();
}

// plug into GPIO-A
void setupAnalog() {
  pinMode(IO_ENABLE,OUTPUT);
  digitalWrite(IO_ENABLE,LOW);
  pinMode(TEMP_IO,INPUT);
  pinMode(HUMI_IO,INPUT);
  analogSetAttenuation(ADC_11db);
}
