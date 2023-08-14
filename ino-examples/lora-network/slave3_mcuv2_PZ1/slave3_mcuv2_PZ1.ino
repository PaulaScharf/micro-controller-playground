

/*******************************************************************************
 * Copyright (c) 2015 Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI Corporation
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This script sends sensor data from the integrated MPU6050 via Lora.
 * The data is only being send, if the device receives a Lora message with its name,
 * which is "slave3".
 *******************************************************************************/

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

char* slave_name = "slave3";

#include "Freenove_WS2812_Lib_for_ESP32.h"
#define LED_PIN 1
Freenove_ESP32_WS2812 led = Freenove_ESP32_WS2812(1, LED_PIN, 0, TYPE_GRB);

// we formerly would check this configuration; but now there is a flag,
// in the LMIC, LMIC.noRXIQinversion;
// if we set that during init, we get the same effect.  If
// DISABLE_INVERT_IQ_ON_RX is defined, it means that LMIC.noRXIQinversion is
// treated as always set.
//
// #if !defined(DISABLE_INVERT_IQ_ON_RX)
// #error This example requires DISABLE_INVERT_IQ_ON_RX to be set. Update \
//        lmic_project_config.h in arduino-lmic/project_config to set it.
// #endif

// How often to send a packet. Note that this sketch bypasses the normal
// LMIC duty cycle limiting, so when you change anything in this sketch
// (payload length, frequency, spreading factor), be sure to check if
// this interval should not also be increased.
// See this spreadsheet for an easy airtime and duty cycle calculator:
// https://docs.google.com/spreadsheets/d/1voGAtQAjC1qBmaVuP1ApNKs1ekgUjavHuVQIXyYSvNc
#define TX_INTERVAL 2000

#define VCC_ENABLE 41

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 34,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = LMIC_UNUSED_PIN,
  .dio = {33, 33, LMIC_UNUSED_PIN},
};


// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in arduino-lmoc/project_config/lmic_project_config.h,
// otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

void onEvent (ev_t ev) {
}

osjob_t txjob;
osjob_t timeoutjob;
static void tx_func (osjob_t* job);

void setLED(uint8_t r,uint8_t g,uint8_t b) {
  led.setLedColorData(0, r, g, b);
  led.show();
}

// Transmit the given string and call the given function afterwards
void tx(const char *str, osjobcb_t func) {
  os_radio(RADIO_RST); // Stop RX first
  delay(1); // Wait a bit, without this os_radio below asserts, apparently because the state hasn't changed yet
  LMIC.dataLen = 0;
  while (*str)
    LMIC.frame[LMIC.dataLen++] = *str++;
  LMIC.osjob.func = func;
  os_radio(RADIO_TX);
}

// Enable rx mode and call func when a packet is received
void rx(osjobcb_t func) {
  LMIC.osjob.func = func;
  LMIC.rxtime = os_getTime(); // RX _now_
  // Enable "continuous" RX (e.g. without a timeout, still stops after
  // receiving a packet)
  os_radio(RADIO_RXON);
}

static void rxtimeout_func(osjob_t *job) {
  setLED(255,0,0); // red
}

static void rx_func (osjob_t* job) {
  // Blink once to confirm reception and then keep the led on
  setLED(0,0,255); // blue
  delay(200);
  setLED(0,255,0); // green

  // Timeout RX (i.e. update led status) after 3 periods without RX
  os_setTimedCallback(&timeoutjob, os_getTime() + ms2osticks(3*TX_INTERVAL), rxtimeout_func);

  if(strncmp(slave_name, (char*) LMIC.frame, strlen(slave_name)) == 0) {
    // Reschedule TX so that it should not collide with the other side's
    // next TX
    os_setTimedCallback(&txjob, os_getTime() + ms2osticks(TX_INTERVAL/2), tx_func);
    Serial.println("Heard name. Sending sensor data...");
  } else {
    // Serial.println("Unknown signal");
  }

  // Restart RX
  rx(rx_func);
}

static void txdone_func (osjob_t* job) {
  rx(rx_func);
}

// log text to USART and toggle LED
static void tx_func (osjob_t* job) {

  // Blink once to confirm reception and then keep the led on
  setLED(255,255,0); // blue
  delay(200);
  setLED(0,255,0); // green

  float mpu6050_values[3];
  getMPU6050Values(mpu6050_values);
  String result = "m" + String(mpu6050_values[0]) + "," + String(mpu6050_values[1]) + "," + String(mpu6050_values[2]);
  Serial.println(result);

  tx(result.c_str(), txdone_func);
}

// application entry point
void setup() {
  Serial.begin(115200);
  Serial.println("Starting");
  led.begin();
  led.setBrightness(30);  
  setLED(0,255,0); // green
  delay(200);
  setLED(255,255,255); // blue
  delay(200);
  setLED(0,255,0); // green
  delay(200);

  // LMIC initialize runtime env
  pinMode(VCC_ENABLE, OUTPUT);
  digitalWrite(VCC_ENABLE, LOW);
  delay(1000);

  // initialize runtime env
  os_init();

  setLED(255,255,255); // blue
  delay(200);
  setLED(0,255,0); // green
  delay(2000);

  // Use a frequency in the g3 which allows 10% duty cycling.
  LMIC.freq = 869525000;
  // Use a medium spread factor. This can be increased up to SF12 for
  // better range, but then, the interval should be (significantly)
  // raised to comply with duty cycle limits as well.
  LMIC.datarate = DR_SF9;
  // Maximum TX power
  LMIC.txpow = 27;


  // disable RX IQ inversion
  LMIC.noRXIQinversion = true;

  // This sets CR 4/5, BW125 (except for EU/AS923 DR_SF7B, which uses BW250)
  LMIC.rps = updr2rps(LMIC.datarate);

  Serial.print("Frequency: "); Serial.print(LMIC.freq / 1000000);
            Serial.print("."); Serial.print((LMIC.freq / 100000) % 10);
            Serial.print("MHz");
  Serial.print("  LMIC.datarate: "); Serial.print(LMIC.datarate);
  Serial.print("  LMIC.txpow: "); Serial.println(LMIC.txpow);

  // This sets CR 4/5, BW125 (except for DR_SF7B, which uses BW250)
  LMIC.rps = updr2rps(LMIC.datarate);

  // disable RX IQ inversion
  LMIC.noRXIQinversion = true;

  setupMPU6050();
  Serial.println("MPU6050 was successfully initialized");

  Serial.println("Setup complete. Waiting for signal...");
  Serial.flush();

  // setup initial job
  rx(rx_func);
}

void loop() {
  // execute scheduled jobs and events
  os_runloop_once();
}


