#include "log.h"
#include "oled.h"
#include <WiFi.h>
#include <HTTPClient.h>

void setupWiFi(const char* ssid, const char* password) {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  displayMessage("Connecting to Wi-Fi\n\n" + String(ssid));
  delay(2000);
  
  WiFi.begin(ssid, password);
  int attempts = 0;
  int max = 30;
  
  while (WiFi.status() != WL_CONNECTED && attempts < max) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    displayMessage("WiFi Connected\nsuccessfully!\n\nIP Address\n" + WiFi.localIP().toString());
    delay(5000);

  } else {
    Serial.println("\nWiFi Connection Failed!");
    displayMessage("WiFi Connection Failed!");
    delay(2000);
    // The program will now continue to the loop() 
    // where it can still read sensors, even if it can't upload.
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