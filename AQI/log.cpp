#include "log.h"
#include "oled.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

WiFiMulti wifiMulti;

//List multiple networks
const char* ssid_1 = "";
const char* ssid_2 = "";
const char* ssid_3 = "";
const char* password_1 = "";
const char* password_2 = "";
const char* password_3 = "";

void setupWiFi() {
  // 1. Add your networks to the Multi list
  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);
  wifiMulti.addAP(ssid_3, password_3); 

  Serial.println("Connecting to Wi-Fi...");
  displayMessage("Scanning for\nknown networks...");

  // 2. wifiMulti.run() connects to the available network
  if(wifiMulti.run() == WL_CONNECTED) {
      Serial.println("\nWiFi Connected successfully!");
      Serial.print("Connected to: ");
      Serial.println(WiFi.SSID()); // Prints which one it picked
      Serial.println(WiFi.localIP());

      displayMessage("Connecting to\n" + WiFi.SSID());
      delay(3000);
      displayMessage("WiFi Connected\nsuccessfully!\n\nIP Address\n" + WiFi.localIP().toString());
      delay(3000);
  } else {
      Serial.println("\nWiFi Connection Failed!");
      displayMessage("WiFi\nConnections Failed!");
      delay(2000);
  }
}

void sendToGoogle(const String& scriptUrl, String jsonData) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Begin connection and ensure it follows Google's redirects
    http.begin(scriptUrl); 
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.addHeader("Content-Type", "application/json");
    
    Serial.println("Uploading data: " + jsonData);
    
    // Send the POST request
    int httpResponseCode = http.POST(jsonData);
    
    if (httpResponseCode > 0) {
      Serial.print("Google Response Code: ");
      Serial.println(httpResponseCode); // 200 means OK!
    } else {
      Serial.print("Error sending to Google. HTTP Code: ");
      Serial.println(httpResponseCode);
    }
    
    http.end(); // Free resources
  } else {
    Serial.println("Error: WiFi is disconnected. Cannot upload.");
  }
}
