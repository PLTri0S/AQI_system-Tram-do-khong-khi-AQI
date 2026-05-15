#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#define SD_CS_PIN 5
#define LOG_INTERVAL  600000        // ms between log writes
#define MAX_LOG_FILES 100
#define MAX_FILE_SIZE 1048576UL     // 1 MB
#define INDEX_FILE    "/index.txt"

extern unsigned long lastLogTime;
extern int           currentLogIndex;

// Time Functions
void syncNTP();
String getFormattedTime();

// SD / file logging
void          makeLogFileName(char *buffer, size_t len, int index);
void          saveLogIndex();
void          loadLogIndex();
void          writeHeaderIfNeeded(const char *fileName);
unsigned long getFileSize(const char *fileName);
void          rotateLogFile();
void          appendDataToSD(
                String timestamp,
                int temperature,
                int humidity,
                uint16_t tvoc,
                uint16_t eco2,
                uint16_t pm1,
                uint16_t pm25
              );

#endif // SDCARD_H 