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
String dataStr = "";
char buffer[7];
void setLED(uint8_t r,uint8_t g,uint8_t b) {
  led.setLedColorData(0, r, g, b);
  led.show();
}
void receiveEvent(int bytes) {
  Serial.println("received");
  start = Wire.read();    // read one character from the I2C
  Serial.println(start);
}

float minDistance = 100000;
float angleAtMinDist = 0;
int nsbit = 0;
float prevAngle = 360.0;
float lastRotationTime = millis();
File file;
void setup() {
  Serial.begin(921600);
  while (!Serial) ;
  Serial.println("setting up");
  pinMode(SD_ENABLE,OUTPUT);
  digitalWrite(SD_ENABLE,LOW);
  delay(2000);
  led.begin();
  led.setBrightness(30);  
  // Start the I2C Bus as Slave on address 9
  Wire.begin(9,39,40,100000); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  // setting up SD card
  SPIClass sdspi = SPIClass();
  sdspi.begin(VSPI_SCLK,VSPI_MISO,VSPI_MOSI,VSPI_SS);
  if(!SD.begin(VSPI_SS,sdspi)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  //clear out old data file
  if (SD.exists("/csv.txt")) 
  {
    Serial.println("Removing /csv.txt");
    SD.remove("/csv.txt");
    Serial.println("Done");
  } 
  //write csv headers to file:
  writeFile(SD, "/csv.txt", "Time,Angle,Distance\n");
  Serial.println("finished setting up");
  setLED(60,0,0);
  Serial.println("waiting for start signal...");
  delay(500);
  while (start==0) {
    setLED(60,0,0);
    delay(200);
    setLED(60,60,60);
    delay(200);
  };
  setLED(0,0,60);
  Serial1.begin(115200,SERIAL_8N1, RX, TX);
  Serial.println("starting Lidar...");
  lidar.begin(Serial1);

  // set pin modes
  pinMode(RPLIDAR_MOTOR, OUTPUT);
  digitalWrite(RPLIDAR_MOTOR,HIGH);
  file = SD.open("/csv.txt", FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  while(start==1) {
    analogWrite(RPLIDAR_MOTOR, analogRead(A0));
    if (IS_OK(lidar.waitPoint())) {
      dataStr = "";
      //convert floats to string and assemble c-type char string for writing:
      dataStr += String(millis()) + ", ";//add it onto the end
      //perform data processing here...
      float distance = lidar.getCurrentPoint().distance;
      float angle = lidar.getCurrentPoint().angle;
      dataStr += String(angle) + ", ";//add it onto the end
      dataStr += String(distance) + ", ";//add it onto the end
      dataStr += "\n";
      if(!file.print(dataStr)){
        Serial.println("Append failed");
      }
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
  file.close();
}
void loop() {
  
}