#include <Wire.h>
#include "BME280.h"
#include "SGP30.h"
#include "pms5003.h"
#include "oled.h"
#include "log.h"
#include "sd_card.h"
#include "sht30.h"

#define SDA_PIN 21 
#define SCL_PIN 22

const String GOOGLE_SCRIPT_URL = "";

unsigned long lastUploadTime = 0;
const unsigned long uploadInterval = 300000;

unsigned long lastSensorReadTime = 0;
const unsigned long sensorInterval = 1000;   // 1 second for SGP30/PMS polling

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);

  initBME(); 
  initSGP();
  initPMS();
  initSHT();
  setupOLED();

  setupWiFi();
  syncNTP();
  
  showBootAnimation();

  Serial.println("Starting circular SD sensor logger...");

  // SD card
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
  } else {
    Serial.println("SD card OK");
    loadLogIndex();
    char fileName[20];
    makeLogFileName(fileName, sizeof(fileName), currentLogIndex);
    writeHeaderIfNeeded(fileName);
    Serial.print("Current log file: ");
    Serial.println(fileName);
  }

  Serial.println("Logger ready.\n-- Hoàn tất quá trình khởi động --");
  delay(2000);
}

void loop() { 
  updatePMS(); // Pulls data from the PMS buffer once

  unsigned long currentMillis = millis();

  if (currentMillis - lastSensorReadTime >= sensorInterval) {
    lastSensorReadTime = currentMillis;

    float Temp_bme = getBMETemp();
    float Hum_bme = getBMEHum();

    float Temp_sht = getSHTTemp();
    float Hum_sht = getSHTHum();

    float Temp_avg = (Temp_bme + Temp_sht) / 2;
    float Hum_avg = (Hum_bme + Hum_sht) / 2;
  
    // SGP30 Compensation & Read
    applySGPCompensation(Temp_avg, Hum_avg);
    readSGP();

    // Print to Serial
    Serial.printf("Temp: %.2f°C \t\t Hum: %.2f%%\n", Temp_avg, Hum_avg);
    Serial.printf("TVOC: %d ppb \t\t eCO2: %d ppm\n", get_TVOC(), get_eCO2());
    Serial.printf("PM1.0: %d Ug \t\t PM2.5: %d Ug\n\n", get_PM1_0(), get_PM2_5());

    //Hiển thị trên màn OLED
    updateDisplay(Temp_avg, Hum_avg, get_TVOC(), get_eCO2(), get_PM2_5(), get_PM1_0());

    //SD CARD
    if (currentMillis - lastLogTime >= LOG_INTERVAL) {
      lastLogTime = currentMillis;

      unsigned long currentTime = getTime();

      Serial.println("========== LOG DATA ==========");
      Serial.println(currentTime);

      appendDataToSD(currentTime, Temp_avg, Hum_avg, get_TVOC(), get_eCO2(), get_PM1_0(), get_PM2_5());
      
      Serial.println("==============================");
    }
  }

  if (currentMillis- lastUploadTime >= uploadInterval) {
    lastUploadTime = currentMillis;

    String jsonData = "{";
    jsonData += "\"PM25\":" + String(get_PM2_5()) + ","; 
    jsonData += "\"PM10\":" + String(get_PM1_0()) + ",";
    jsonData += "\"Temp\":" + String(int((getSHTTemp() + getBMETemp()) / 2)) + ",";
    jsonData += "\"Hum\":"  + String(int((getSHTHum() + getBMEHum()) / 2)) + ",";
    jsonData += "\"TVOC\":" + String(get_TVOC()) + ",";
    jsonData += "\"eCO2\":" + String(get_eCO2());
    jsonData += "}";

    // Call the function from your new .cpp file
    sendToGoogle(GOOGLE_SCRIPT_URL, jsonData);
    }
}
