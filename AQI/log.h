#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

// Function to connect to your Wi-Fi network
void setupWiFi(const char* ssid, const char* password);

// Function to send the JSON string to Google Sheets
void sendToGoogle(const String& scriptUrl, String jsonData);

#endif