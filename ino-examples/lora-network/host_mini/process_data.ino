void processMessageAndPrint (char* message) {
  char *state1, *state2, *state3;
  char* messagePart = strtok_r((char*) message, ":", &state1); 
  messagePart = strtok_r(NULL, ":", &state1); 
  if (messagePart != NULL) {
    char* sensorData = strtok_r(messagePart, ";", &state2);
    while (sensorData != NULL) {
      char sensorType = sensorData[0];
      sensorData++;
      char* sensorValue = strtok_r(sensorData, ",", &state3);
      int valueIndex = 0;
      while (sensorValue != NULL) {
        if (sensorType == 't') {
          if (valueIndex == 0) {
            Serial.println("Tof Imager measured: ");
            Serial.print("A minimum distance of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 1) {
            Serial.print(", a maximum distance of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 2) {
            Serial.print(" and ");
            Serial.print(sensorValue);
            Serial.println(" pixel(s) without a valid reading");
            valueIndex = 0;
          }
        } else if (sensorType == 'r') {
          if (valueIndex == 0) {
            Serial.println("Tof Ranger measured: ");
            Serial.print("A signal status of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 1) {
            Serial.print(", a signal strength of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 2) {
            Serial.print(" and a distance of ");
            Serial.println(sensorValue);
            valueIndex = 0;
          }
        } else if ((sensorType == 's') == 1) {
          if (valueIndex == 0) {
            Serial.println("SMT50 measured: ");
            Serial.print("A soil temperature of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 1) {
            Serial.print(", a soil moisture of ");
            Serial.println(sensorValue);
            valueIndex = 0;
          } 
        } else if ((sensorType == 'm') == 1) {
          if (valueIndex == 0) {
            Serial.println("MPU6050 measured: ");
            Serial.print("X angle of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 1) {
            Serial.print(", Y angle of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 2) {
            Serial.print(" and Z angle of ");
            Serial.println(sensorValue);
            valueIndex = 0;
          }
        } else {
          Serial.println("Sensor Type not recognized...");
        }

        valueIndex++;
        sensorValue = strtok_r(NULL, ",", &state3);
      }
      sensorData = strtok_r(NULL, ";", &state2);
    }
  }
}