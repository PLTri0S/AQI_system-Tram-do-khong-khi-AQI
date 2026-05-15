#include "oled.h"

#define OLED_RESET -1 
#define i2c_Address 0x3c // Default I2C address for SH1106 displays

// Construct the SH1106 object
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupOLED() {
  delay(250); // The SH1106 needs a slight delay to power up properly
  
  // Initialize the display
  if(!display.begin(i2c_Address, true)) {
    Serial.println("SH1106 allocation failed");
    for(;;); // Loop forever if it fails
  }
  
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
}

void displayMessage(String message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}

void showBootAnimation() {
  display.clearDisplay();
  
  // 1. Draw the Star (using a scaled up asterisk)
  display.setTextSize(2);
  display.setCursor(58, 10); 
  display.print("*");
  display.display();
  delay(600); 

  // 2. Draw "Air Quality" text
  display.setTextSize(1);
  display.setCursor(30, 30);
  display.print("AIR QUALITY");
  display.display();
  delay(600); 

  // 3. Draw the underline
  display.drawLine(30, 40, 95, 40, SH110X_WHITE);
  display.display();
  delay(1500); 
}

void updateDisplay(int temp, int humidity, uint16_t tvoc, uint16_t eco2, int PM25, int PM10) {
  display.clearDisplay();
  display.setTextSize(1); 

  // ================= COLUMN 1 (LEFT) =================
  display.setCursor(0, 0);
  display.print("Temp ");
  display.print(temp);

  display.setCursor(0, 16);
  display.print("Humidity ");
  display.print(humidity);
  display.print("%");


  display.setCursor(0, 32);
  display.print("eCO2 ");
  display.print(eco2);


  display.setCursor(0, 48);
  display.print("PM2.5 ");
  display.print(PM25);

  // ================= COLUMN 2 (RIGHT) =================
  display.setCursor(64, 32);
  display.print("TVOC ");
  display.print(tvoc);

  display.setCursor(64, 48);
  display.print("PM1.0 ");
  display.print(PM10);


  // Push all the drawing to the screen
  display.display();
}