void processMessageAndPrint (char* message) {
  Serial.println(message);
  char* messagePart = strtok((char*) message, ":"); 
  messagePart = strtok(NULL, ":"); 
  if (messagePart != NULL) {
    Serial.println(messagePart);
    char* sensorData = strtok(messagePart, ";");
    while (sensorData != NULL) {
      Serial.println(sensorData);
      char sensorType = sensorData[0];

      Serial.println(sensorType);
      // memcpy(sensorType, sensorData[0]);
      // char* sensorValue = strtok(sensorData, (char*) sensorData[0]);
      // sensorValue = strtok(NULL, (char*) sensorData[0]);
      sensorData++;
      char* sensorValue = strtok(sensorData, ",");
      int valueIndex = 0;
      while (sensorValue != NULL) {
        Serial.println(sensorValue);
        valueIndex++;
        if (strncmp((char*) sensorType, "t", strlen("t")) == 0)) {
          if (valueIndex == 0) {
            Serial.println("Tof Imager measured: ");
            Serial.print("A minimum distance of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 1) {
            Serial.print(" , a maximum distance of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 2) {
            Serial.print(" and ");
            Serial.print(sensorValue);
            Serial.println(" and pixel without a valid reading");
            valueIndex = 0;
          }
        } else if ((char*) sensorType == "r") {
          if (valueIndex == 0) {
            Serial.println("Tof Ranger measured: ");
            Serial.print("A signal status of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 1) {
            Serial.print(" , a signal strength of ");
            Serial.print(sensorValue);
          } else if (valueIndex == 2) {
            Serial.print(" and a distance of ");
            Serial.println(sensorValue);
            valueIndex = 0;
          }
        } else if ((char*) sensorType == "s") {

        } else if ((char*) sensorType == "m") {

        } else {

        }

        sensorValue = strtok(NULL, ",");
      }
      sensorData = strtok(NULL, ";");
    }
  }
}