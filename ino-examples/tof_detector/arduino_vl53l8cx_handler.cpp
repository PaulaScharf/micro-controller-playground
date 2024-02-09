
#include "vl53l8cx_handler.h"

#include <Arduino.h>
#include <Wire.h>
#include <vl53l8cx_class.h>

#define DEV_I2C Wire
#define LPN_PIN 4
#define I2C_RST_PIN -1
#define PWREN_PIN 2
VL53L8CX sensor_VL53L8CX_top(&DEV_I2C, LPN_PIN, I2C_RST_PIN);

void measure(void);
void print_result(VL53L8CX_ResultsData *Result);
String dataStr = "";

VL53L8CX_DetectionThresholds thresholds[VL53L8CX_NB_THRESHOLDS];
const int res = VL53L8CX_RESOLUTION_8X8;

bool EnableAmbient = false;
bool EnableSignal = false;
char report[256];
volatile int interruptCount = 0;

uint8_t number_of_zones = res;
int8_t i, j, k, l;
uint8_t zones_per_line = (number_of_zones == 16) ? 4 : 8;

#include "constants.h"

// A buffer holding the last 200 sets of 3-channel values
const int RING_BUFFER_SIZE = 1920;
float save_data[RING_BUFFER_SIZE] = {0.0};
// Most recent position in the save_data buffer
int begin_index = 0;
// True if there is not yet enough data to run inference
bool pending_initial_data = true;
// How often we should save a measurement during downsampling
int sample_every_n;
// The number of measurements since we last saved one
int sample_skip_counter = 1;

bool SetupVL53L8CX() {
  // Enable PWREN pin if present
  if (PWREN_PIN >= 0) {
    pinMode(PWREN_PIN, OUTPUT);
    digitalWrite(PWREN_PIN, HIGH);
    delay(10);
  }

  // Initialize I2C bus.
  DEV_I2C.begin(39,40);
  DEV_I2C.setClock(1000000); //Sensor has max I2C freq of 1MHz

  
  // Configure VL53L8CX component.
  sensor_VL53L8CX_top.begin();
  sensor_VL53L8CX_top.init_sensor();
  sensor_VL53L8CX_top.vl53l8cx_set_resolution(res);

  Serial.println("starting to measure");
  // Start Measurements.
  sensor_VL53L8CX_top.vl53l8cx_start_ranging(); 

  return true;
}


VL53L8CX_ResultsData Results;
uint8_t NewDataReady = 0;
uint8_t status;
bool ReadVL53L8CX(float* input,
                       int length, bool reset_buffer) {
                        // Clear the buffer if required, e.g. after a successful prediction
  if (reset_buffer) {
    memset(save_data, 0, RING_BUFFER_SIZE * sizeof(float)*64);
    begin_index = 0;
    pending_initial_data = true;
  }
  bool new_data = false;
  // int currentFrame[res];

  status = sensor_VL53L8CX_top.vl53l8cx_check_data_ready(&NewDataReady);

  if ((!status) && (NewDataReady != 0)) {
    for (j = 0; j < number_of_zones; j ++)
    {
      //perform data processing here...
      if((long)(&Results)->target_status[j] !=255){
        save_data[begin_index++] = (long)(&Results)->distance_mm[j];
      }
    }

    NewDataReady = 0;
  }

  // If we reached the end of the circle buffer, reset
  if (begin_index >= 1500) {
    begin_index = 0;
  }

  // Check if we are ready for prediction or still pending more initial data
  if (pending_initial_data && begin_index >= 1280) {
    pending_initial_data = false;
  }

  // Return if we don't have enough data
  if (pending_initial_data) {
    return false;
  }

  // Copy the requested number of bytes to the provided input tensor
  for (int i = 0; i < length; ++i) {
    int ring_array_index = begin_index + i - length;
    if (ring_array_index < 0) {
      ring_array_index += 1500;
    }
    input[i] = save_data[ring_array_index];
  }

  return true;
}
