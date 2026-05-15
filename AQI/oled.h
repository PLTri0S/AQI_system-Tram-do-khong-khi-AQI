#ifndef OLED_H
#define OLED_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h> // Swapped to SH110X library
#include <Wire.h>

// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Initialize the OLED display
void setupOLED();

void displayMessage(String message);

// Play the startup animation
void showBootAnimation();

// Update the 2-column data dashboard
void updateDisplay(int temp, int humidity, uint16_t tvoc, uint16_t eco2, int PM25, int PM10);

#endif