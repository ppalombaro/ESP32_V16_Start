/* ThemeManager.cpp
   Theme control implementation
   VERSION: V16.1.2-2026-01-08T15:00:00Z
*/

#include "ThemeManager.h"
#include "MatrixDisplay.h"
#include "ContentManager.h"
#include "Logger.h"

ThemeManager::ThemeManager() {}

void ThemeManager::begin(MatrixDisplay* display, ContentManager* content) {
    disp = display;
    contentMgr = content;
    currentTheme = 0;
    Logger::instance().log("[ThemeManager] Initialized");
}

void ThemeManager::setTheme(uint8_t theme) {
    currentTheme = theme;
    Logger::instance().log("[ThemeManager] Theme set to " + String(theme));
}

void ThemeManager::update() {
    // V16.1.2 - Future: auto-cycling, theme-based updates
}

void ThemeManager::renderContent(uint16_t contentId) {
    if (contentMgr) {
        contentMgr->renderContent(contentId);
    }
}

// V16.2.2-2026-01-10T19:00:00Z - Color accessors for themed content
CRGB ThemeManager::getColor1() const {
    // Theme-specific primary colors
    switch (currentTheme) {
        case 0: return CRGB::Red;      // Christmas
        case 1: return CRGB::Orange;   // Halloween
        case 2: return CRGB::Orange;   // Thanksgiving
        case 3: return CRGB::Gold;     // New Year
        case 4: return CRGB::Red;      // OSU
        default: return CRGB::White;
    }
}

CRGB ThemeManager::getColor2() const {
    // Theme-specific secondary colors
    switch (currentTheme) {
        case 0: return CRGB::Green;    // Christmas
        case 1: return CRGB::Purple;   // Halloween
        case 2: return CRGB::Brown;    // Thanksgiving
        case 3: return CRGB::Silver;   // New Year
        case 4: return CRGB::Grey;     // OSU
        default: return CRGB::Blue;
    }
}

CRGB ThemeManager::getColor3() const {
    // Theme-specific accent colors
    switch (currentTheme) {
        case 0: return CRGB::White;    // Christmas
        case 1: return CRGB::Green;    // Halloween
        case 2: return CRGB::Yellow;   // Thanksgiving
        case 3: return CRGB::Blue;     // New Year
        case 4: return CRGB::White;    // OSU
        default: return CRGB::Red;
    }
}
