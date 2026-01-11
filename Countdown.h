/* Countdown.h
   Countdown display system with JSON configuration
   VERSION: V16.2.0-2026-01-10T18:05:00Z - Initial implementation
   
   Supports JSON-driven countdown timers with theme colors
   Colors: Header=theme1, Box=theme2, Numbers=theme3
   Flashes "00" when target date reached/passed
*/

#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <time.h>

class MatrixDisplay;
class ThemeManager;
class NTPClient;

class Countdown {
public:
    Countdown(MatrixDisplay* display, ThemeManager* themeMgr, NTPClient* ntp);
    
    // Load countdown configuration from JSON
    bool loadFromJSON(const String& jsonPath);
    
    // Animation control
    void begin();
    void update();
    
    // Manual configuration
    void setTargetDate(time_t targetEpoch);
    
private:
    MatrixDisplay* disp;
    ThemeManager* themes;
    NTPClient* ntpClient;
    
    time_t targetTime;
    unsigned long lastUpdate;
    bool flashState;
    unsigned long lastFlash;
    
    // V16.2.0-2026-01-10T18:05:00Z - 3x5 digit font
    static const uint8_t DIGIT_3X5[][5];
    
    void drawDigit(int matrix, int x, int y, int digit, CRGB color);
    void drawLabel(int matrix, int x, int y, char label, CRGB color);
    void drawRectBorder(int matrix, int x1, int y1, int x2, int y2, CRGB color);
    void drawBox(int matrix, int x, int y, char label, long value, bool shouldFlash);
    
    // V16.2.0-2026-01-10T18:30:00Z - Parse human-readable date to Unix timestamp
    time_t parseHumanDate(const String& dateStr);
};
