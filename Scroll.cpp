/* Scroll.cpp
   Scrolling text display implementation
   VERSION: V16.2.0-2026-01-10T18:00:00Z - Initial implementation with theme color cycling
*/

#include "Scroll.h"
#include "MatrixDisplay.h"
#include "ThemeManager.h"
#include "Logger.h"
#include <ArduinoJson.h>
#include "esp_partition.h"
#include "esp_spi_flash.h"

#define DATA_PARTITION_OFFSET 0x290000

// V16.2.0-2026-01-10T18:00:00Z - 5x7 font definition (ASCII 32-90)
const uint8_t Scroll::FONT_5X7[][5] = {
  {0x00, 0x00, 0x00, 0x00, 0x00}, // space (32)
  {0x00, 0x00, 0x5F, 0x00, 0x00}, // ! (33)
  {0x00, 0x07, 0x00, 0x07, 0x00}, // " (34)
  {0x14, 0x7F, 0x14, 0x7F, 0x14}, // # (35)
  {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $ (36)
  {0x23, 0x13, 0x08, 0x64, 0x62}, // % (37)
  {0x36, 0x49, 0x55, 0x22, 0x50}, // & (38)
  {0x00, 0x05, 0x03, 0x00, 0x00}, // ' (39)
  {0x00, 0x1C, 0x22, 0x41, 0x00}, // ( (40)
  {0x00, 0x41, 0x22, 0x1C, 0x00}, // ) (41)
  {0x14, 0x08, 0x3E, 0x08, 0x14}, // * (42)
  {0x08, 0x08, 0x3E, 0x08, 0x08}, // + (43)
  {0x00, 0x50, 0x30, 0x00, 0x00}, // , (44)
  {0x08, 0x08, 0x08, 0x08, 0x08}, // - (45)
  {0x00, 0x60, 0x60, 0x00, 0x00}, // . (46)
  {0x20, 0x10, 0x08, 0x04, 0x02}, // / (47)
  {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0 (48)
  {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1 (49)
  {0x42, 0x61, 0x51, 0x49, 0x46}, // 2 (50)
  {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3 (51)
  {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4 (52)
  {0x27, 0x45, 0x45, 0x45, 0x39}, // 5 (53)
  {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6 (54)
  {0x01, 0x71, 0x09, 0x05, 0x03}, // 7 (55)
  {0x36, 0x49, 0x49, 0x49, 0x36}, // 8 (56)
  {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9 (57)
  {0x00, 0x36, 0x36, 0x00, 0x00}, // : (58)
  {0x00, 0x56, 0x36, 0x00, 0x00}, // ; (59)
  {0x08, 0x14, 0x22, 0x41, 0x00}, // < (60)
  {0x14, 0x14, 0x14, 0x14, 0x14}, // = (61)
  {0x00, 0x41, 0x22, 0x14, 0x08}, // > (62)
  {0x02, 0x01, 0x51, 0x09, 0x06}, // ? (63)
  {0x32, 0x49, 0x79, 0x41, 0x3E}, // @ (64)
  {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A (65)
  {0x7F, 0x49, 0x49, 0x49, 0x36}, // B (66)
  {0x3E, 0x41, 0x41, 0x41, 0x22}, // C (67)
  {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D (68)
  {0x7F, 0x49, 0x49, 0x49, 0x41}, // E (69)
  {0x7F, 0x09, 0x09, 0x09, 0x01}, // F (70)
  {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G (71)
  {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H (72)
  {0x00, 0x41, 0x7F, 0x41, 0x00}, // I (73)
  {0x20, 0x40, 0x41, 0x3F, 0x01}, // J (74)
  {0x7F, 0x08, 0x14, 0x22, 0x41}, // K (75)
  {0x7F, 0x40, 0x40, 0x40, 0x40}, // L (76)
  {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M (77)
  {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N (78)
  {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O (79)
  {0x7F, 0x09, 0x09, 0x09, 0x06}, // P (80)
  {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q (81)
  {0x7F, 0x09, 0x19, 0x29, 0x46}, // R (82)
  {0x46, 0x49, 0x49, 0x49, 0x31}, // S (83)
  {0x01, 0x01, 0x7F, 0x01, 0x01}, // T (84)
  {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U (85)
  {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V (86)
  {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W (87)
  {0x63, 0x14, 0x08, 0x14, 0x63}, // X (88)
  {0x07, 0x08, 0x70, 0x08, 0x07}, // Y (89)
  {0x61, 0x51, 0x49, 0x45, 0x43}  // Z (90)
};

Scroll::Scroll(MatrixDisplay* display, ThemeManager* themeMgr) 
    : disp(display), themes(themeMgr), scrollSpeed(50), scrollPos(0), 
      lastUpdate(0), currentColorIndex(0), repeatCount(0) {
    scrollText = "HELLO";
}

bool Scroll::loadFromJSON(const String& jsonPath) {
    // V16.2.0-2026-01-10T18:00:00Z - Read JSON from flash storage
    Logger::instance().log("[Scroll] Loading: " + jsonPath);
    
    // Find file in flash storage (same method as ContentManager)
    uint32_t flash_addr = DATA_PARTITION_OFFSET;
    uint32_t file_count = 0;
    
    if (esp_flash_read(NULL, &file_count, flash_addr, 4) != ESP_OK) {
        Logger::instance().log("[Scroll] Failed to read file count");
        return false;
    }
    
    flash_addr += 4;
    
    // Search for the file
    for (uint32_t i = 0; i < file_count; i++) {
        uint16_t path_len = 0;
        if (esp_flash_read(NULL, &path_len, flash_addr, 2) != ESP_OK) break;
        flash_addr += 2;
        
        char path_buf[256];
        if (esp_flash_read(NULL, path_buf, flash_addr, path_len) != ESP_OK) break;
        flash_addr += path_len;
        path_buf[path_len] = '\0';
        
        uint32_t content_len = 0;
        if (esp_flash_read(NULL, &content_len, flash_addr, 4) != ESP_OK) break;
        flash_addr += 4;
        
        // Check if this is our file
        if (String(path_buf) == jsonPath) {
            // Found it! Read content
            char* content = new char[content_len + 1];
            if (esp_flash_read(NULL, content, flash_addr, content_len) == ESP_OK) {
                content[content_len] = '\0';
                
                // Parse JSON
                DynamicJsonDocument doc(1024);
                DeserializationError error = deserializeJson(doc, content);
                delete[] content;
                
                if (error) {
                    Logger::instance().log("[Scroll] JSON parse error: " + String(error.c_str()));
                    return false;
                }
                
                // Extract configuration
                if (doc.containsKey("text")) {
                    scrollText = doc["text"].as<String>();
                }
                if (doc.containsKey("speed")) {
                    scrollSpeed = doc["speed"];
                }
                
                Logger::instance().log("[Scroll] Loaded: '" + scrollText + "' @ " + String(scrollSpeed) + "ms");
                return true;
            }
            delete[] content;
            return false;
        }
        
        // Skip to next file
        flash_addr += content_len;
        uint32_t padding = (512 - (flash_addr % 512)) % 512;
        flash_addr += padding;
    }
    
    Logger::instance().log("[Scroll] File not found: " + jsonPath);
    return false;
}

void Scroll::begin() {
    scrollPos = 50;  // V16.2.0-2026-01-10T18:00:00Z - Start off right edge (50 pixels = 2 matrices)
    currentColorIndex = 0;
    repeatCount = 0;
    lastUpdate = millis();
}

void Scroll::update() {
    unsigned long now = millis();
    if (now - lastUpdate < scrollSpeed) return;
    lastUpdate = now;
    
    disp->clear();
    
    // V16.2.0-2026-01-10T18:00:00Z - Get current theme color
    CRGB color = getCurrentColor();
    
    int charSpacing = 6;  // 5 pixels width + 1 pixel gap
    int totalWidth = scrollText.length() * charSpacing;
    
    // Draw each character
    for (size_t i = 0; i < scrollText.length(); i++) {
        char c = scrollText.charAt(i);
        int charPos = scrollPos + (i * charSpacing);
        
        // Only draw if visible on either matrix
        if (charPos >= -charSpacing && charPos < 50) {
            drawCharacter(c, charPos, color);
        }
    }
    
    // Move position
    scrollPos--;
    if (scrollPos < -totalWidth) {
        scrollPos = 50;  // Reset to right edge
        repeatCount++;
        currentColorIndex = (currentColorIndex + 1) % 3;  // V16.2.0-2026-01-10T18:00:00Z - Cycle colors
    }
    
    disp->show();
}

void Scroll::drawCharacter(char c, int globalX, CRGB color) {
    // V16.2.0-2026-01-10T18:00:00Z - Draw character at global X position (0-49 across both matrices)
    int fontIndex = -1;
    if (c >= 32 && c <= 90) {
        fontIndex = c - 32;
    }
    
    if (fontIndex < 0) return;
    
    // Draw 5x7 character
    for (int col = 0; col < 5; col++) {
        int x = globalX + col;
        
        // Determine which matrix and local position
        int matrix = 0;
        int localX = x;
        if (x >= 25) {
            matrix = 1;
            localX = x - 25;
        }
        
        // Only draw if in valid range
        if (x >= 0 && x < 50 && localX >= 0 && localX < 25) {
            uint8_t columnData = FONT_5X7[fontIndex][col];
            
            // Draw vertical column
            for (int row = 0; row < 7; row++) {
                if (columnData & (1 << row)) {
                    int y = 9 + row;  // Center vertically (rows 9-15)
                    if (y < 20) {
                        disp->setPixel(matrix, localX, y, color);
                    }
                }
            }
        }
    }
}

CRGB Scroll::getCurrentColor() {
    // V16.2.0-2026-01-10T18:00:00Z - Cycle through theme colors (1, 2, 3)
    switch (currentColorIndex) {
        case 0: return themes->getColor1();
        case 1: return themes->getColor2();
        case 2: return themes->getColor3();
        default: return themes->getColor1();
    }
}

void Scroll::setText(const String& text) {
    scrollText = text;
}

void Scroll::setSpeed(int speedMs) {
    scrollSpeed = speedMs;
}
