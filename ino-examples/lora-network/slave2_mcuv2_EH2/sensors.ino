int i;
int dat[32]={0};

#define TEMP_IO 3
#define HUMI_IO 2
#define IO_ENABLE 8

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
  arr[0] = -1; // status
  arr[1] = -1; // strength
  arr[2] = -1; // distance in mm
  if(Serial1.available()>=0)
  {
    for(i=0;i<32;i++)
    {
      dat[i]=Serial1.read();
    }
    for(i=0;i<16;i++)
    {
      if(dat[i]==0x57&&dat[i+1]==0&&dat[i+2]==0xff&&dat[i+3]==0)
      {
        if(dat[i+12]+dat[i+13]*255==0)
        {
          Serial.println("Out of range!");
        }
        else
        { 
          arr[0]=dat[i+11];
          arr[1]=dat[i+12]+dat[i+13]*255;
          arr[2]=dat[i+8]+dat[i+9]*255;
        }
          break; 
      } 
    }
  } else {
    Serial.print("Serial not available. Only received: ");
    Serial.println(Serial1.available());
  }
}

/*************************

Sensor Setup

*************************/
// plug into UART, but use the 5V interface to supply power via the VCC
void setupToFRanger() {
  Serial1.begin(115200,SERIAL_8N1, RX, TX);
}

// plug into GPIO-A
void setupAnalog() {
  pinMode(IO_ENABLE,OUTPUT);
  digitalWrite(IO_ENABLE,LOW);
  pinMode(TEMP_IO,INPUT);
  pinMode(HUMI_IO,INPUT);
  analogSetAttenuation(ADC_11db);
}
