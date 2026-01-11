/* Scroll.h
   Scrolling text display system with JSON configuration
   VERSION: V16.2.0-2026-01-10T18:00:00Z - Initial implementation
   
   Supports JSON-driven scrolling text with theme color cycling
*/

#pragma once

#include <Arduino.h>
#include <FastLED.h>

class MatrixDisplay;
class ThemeManager;

class Scroll {
public:
    Scroll(MatrixDisplay* display, ThemeManager* themeMgr);
    
    // Load scroll configuration from JSON
    bool loadFromJSON(const String& jsonPath);
    
    // Animation control
    void begin();
    void update();
    
    // Manual configuration
    void setText(const String& text);
    void setSpeed(int speedMs);
    
private:
    MatrixDisplay* disp;
    ThemeManager* themes;
    
    String scrollText;
    int scrollSpeed;
    int scrollPos;
    unsigned long lastUpdate;
    int currentColorIndex;  // V16.2.0-2026-01-10T18:00:00Z - Cycle through theme colors
    int repeatCount;        // V16.2.0-2026-01-10T18:00:00Z - Track color changes
    
    // V16.2.0-2026-01-10T18:00:00Z - 5x7 font for text rendering
    static const uint8_t FONT_5X7[][5];
    
    void drawCharacter(char c, int globalX, CRGB color);
    CRGB getCurrentColor();
};
