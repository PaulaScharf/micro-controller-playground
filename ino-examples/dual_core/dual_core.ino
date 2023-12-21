/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;

void setup() {
  Serial.begin(115200); 
  delay(2500);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */

    //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task3code,   /* Task function. */
                    "Task3",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task3,      /* Task handle to keep track of created task */
                    2);          /* pin task to core 1 */
    delay(500); 
}

long a = 0;
long b = 0;
long c = 0;

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  delay(7000);

  for(;;){
    Serial.println(millis()-a);
    a = millis();
    delay(1000);
    Serial.println(millis()-a);
    a = millis();
    delay(1000);
  } 
}

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  delay(7000);

  for(;;){
    Serial.println(millis()-b);
    b = millis();
    delay(700);
    Serial.println(millis()-b);
    b = millis();
    delay(700);
  }
}

//Task2code: blinks an LED every 700 ms
void Task3code( void * pvParameters ){
  Serial.print("Task3 running on core ");
  Serial.println(xPortGetCoreID());
  delay(7000);

  for(;;){
    Serial.println(millis()-c);
    c = millis();
    delay(200);
    Serial.println(millis()-c);
    c = millis();
    delay(200);
  }
}

void loop() {
  
}
