/* ThemeManager.h
   Theme control and content rendering
   VERSION: V16.2.2-2026-01-10T19:00:00Z - Added color accessors for themed content
*/

#pragma once

#include <Arduino.h>
#include <FastLED.h>

class MatrixDisplay;
class ContentManager;

class ThemeManager {
public:
    ThemeManager();

    void begin(MatrixDisplay* display, ContentManager* content);
    void update();

    void setTheme(uint8_t theme);
    uint8_t getCurrentTheme() const { return currentTheme; }
    
    void renderContent(uint16_t contentId); // V16.1.2
    
    // V16.2.2-2026-01-10T19:00:00Z - Color accessors for themed content
    CRGB getColor1() const;
    CRGB getColor2() const;
    CRGB getColor3() const;

private:
    MatrixDisplay* disp = nullptr;
    ContentManager* contentMgr = nullptr;
    uint8_t currentTheme = 0;
};
