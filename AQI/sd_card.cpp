#include "sd_card.h"
#include <time.h>

unsigned long lastLogTime    = 0;
int currentLogIndex = 0;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;      
const int   daylightOffset_sec = 0;

void syncNTP() {
  Serial.println("Syncing time with NTP...");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

String getFormattedTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    return "00/00/00 00:00:00"; 
  }
  char timeStringBuff[30];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%d/%m/%y %H:%M:%S", &timeinfo);
  return String(timeStringBuff);
}

// ================= FILE LOG =================

void makeLogFileName(char *buffer, size_t len, int index) {
  snprintf(buffer, len, "/log%03d.csv", index);
}

void saveLogIndex() {
  if (SD.exists(INDEX_FILE)) SD.remove(INDEX_FILE);

  File file = SD.open(INDEX_FILE, FILE_WRITE);
  if (file) {
    file.print(currentLogIndex);
    file.close();
  } else {
    Serial.println("Failed to save log index.");
  }
}

void loadLogIndex() {
  if (!SD.exists(INDEX_FILE)) {
    currentLogIndex = 0;
    saveLogIndex();
    return;
  }

  File file = SD.open(INDEX_FILE, FILE_READ);
  if (!file) {
    currentLogIndex = 0;
    return;
  }

  String content = file.readString();
  file.close();

  currentLogIndex = content.toInt();

  if (currentLogIndex < 0 || currentLogIndex >= MAX_LOG_FILES) {
    currentLogIndex = 0;
    saveLogIndex();
  }
}

void writeHeaderIfNeeded(const char *fileName) {
  bool needHeader = false;

  if (!SD.exists(fileName)) {
    needHeader = true;
  } else {
    File file = SD.open(fileName, FILE_READ);
    if (file) {
      if (file.size() == 0) needHeader = true;
      file.close();
    } else {
      needHeader = true;
    }
  }

  if (needHeader) {
    File file = SD.open(fileName, FILE_WRITE);
    if (file) {
      file.println("time,pm2_5_ugm3,pm1_0_ugm3,temperature_c,humidity_percent,tvoc_ppb,eco2_ppm");
      file.close();
      Serial.print("Header written to ");
      Serial.println(fileName);
    } else {
      Serial.print("Failed to write header to ");
      Serial.println(fileName);
    }
  }
}

unsigned long getFileSize(const char *fileName) {
  if (!SD.exists(fileName)) return 0;

  File file = SD.open(fileName, FILE_READ);
  if (!file) return 0;

  unsigned long size = file.size();
  file.close();
  return size;
}

void rotateLogFile() {
  currentLogIndex++;
  if (currentLogIndex >= MAX_LOG_FILES) currentLogIndex = 0;

  saveLogIndex();

  char fileName[20];
  makeLogFileName(fileName, sizeof(fileName), currentLogIndex);

  if (SD.exists(fileName)) {
    SD.remove(fileName);
    Serial.print("Old file removed: ");
    Serial.println(fileName);
  }

  writeHeaderIfNeeded(fileName);

  Serial.print("Switched to new log file: ");
  Serial.println(fileName);
}

void appendDataToSD(
  String timestamp,
  int temperature,
  int humidity,
  uint16_t tvoc,
  uint16_t eco2,
  uint16_t pm1,
  uint16_t pm25
) {
  char fileName[20];
  makeLogFileName(fileName, sizeof(fileName), currentLogIndex);

  if (getFileSize(fileName) >= MAX_FILE_SIZE) {
    Serial.println("Current log file is full. Rotating...");
    rotateLogFile();
    makeLogFileName(fileName, sizeof(fileName), currentLogIndex);
  }

  writeHeaderIfNeeded(fileName);

  File file = SD.open(fileName, FILE_APPEND);
  if (!file) {
    Serial.print("Failed to open ");
    Serial.print(fileName);
    Serial.println(" for writing.");
    return;
  }

  file.print(timestamp);   file.print(",");
  file.print(pm25);        file.print(",");
  file.print(pm1);         file.print(",");
  file.print(temperature); file.print(",");
  file.print(humidity);    file.print(",");
  file.print(tvoc);        file.print(",");
  file.println(eco2);      
  

  file.close();

  Serial.print("Data written to ");
  Serial.println(fileName);
}