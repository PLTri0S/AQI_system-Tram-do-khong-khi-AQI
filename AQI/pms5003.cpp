/*
  PM2.5 Demo
  pm25-demo.ino
  Demonstrates operation of PM2.5 Particulate Matter Sensor
  ESP32 Serial Port (RX = 16, TX = 17)
  Derived from howtoelectronics.com - https://how2electronics.com/interfacing-pms5003-air-quality-sensor-arduino/

  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/
#include "pms5003.h"
// Serial Port connections for PM2.5 Sensor
#define RXD2 16 // To sensor TXD
#define TXD2 17 // To sensor RXD

void initPMS() {
  // our debugging output
  // Set up UART connection
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

struct pms5003data data;

// Cache variables
int last_pm25 = 0;
int last_pm10 = 0;

void updatePMS() {
  if (readPMSdata(&Serial1)) {
    // If reading was successful, update the cache!
    last_pm25 = data.pm25_env;
    last_pm10 = data.pm10_env;
  }
}

int get_PM2_5() {
  return last_pm25;
}

int get_PM1_0() {
  return last_pm10;
}

boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }

  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  /* debugging
    for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
    }
    Serial.println();
  */

  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}