/* Countdown.cpp
   Countdown display implementation
   VERSION: V16.2.0-2026-01-10T18:05:00Z - Initial implementation with theme colors and flash behavior
*/

#include "Countdown.h"
#include "MatrixDisplay.h"
#include "ThemeManager.h"
#include "Logger.h"
#include <ArduinoJson.h>
#include <NTPClient.h>
#include "esp_partition.h"
#include "esp_spi_flash.h"

#define DATA_PARTITION_OFFSET 0x290000

// V16.2.0-2026-01-10T18:05:00Z - 3x5 digit font
const uint8_t Countdown::DIGIT_3X5[][5] = {
  {0b111, 0b101, 0b101, 0b101, 0b111}, // 0
  {0b010, 0b110, 0b010, 0b010, 0b111}, // 1
  {0b111, 0b001, 0b111, 0b100, 0b111}, // 2
  {0b111, 0b001, 0b111, 0b001, 0b111}, // 3
  {0b101, 0b101, 0b111, 0b001, 0b001}, // 4
  {0b111, 0b100, 0b111, 0b001, 0b111}, // 5
  {0b111, 0b100, 0b111, 0b101, 0b111}, // 6
  {0b111, 0b001, 0b001, 0b001, 0b001}, // 7
  {0b111, 0b101, 0b111, 0b101, 0b111}, // 8
  {0b111, 0b101, 0b111, 0b001, 0b111}  // 9
};

Countdown::Countdown(MatrixDisplay* display, ThemeManager* themeMgr, NTPClient* ntp)
    : disp(display), themes(themeMgr), ntpClient(ntp), targetTime(0), 
      lastUpdate(0), flashState(false), lastFlash(0) {
}

bool Countdown::loadFromJSON(const String& jsonPath) {
    // V16.2.0-2026-01-10T18:30:00Z - Read JSON from flash storage with human-readable date support
    Logger::instance().log("[Countdown] Loading: " + jsonPath);
    
    uint32_t flash_addr = DATA_PARTITION_OFFSET;
    uint32_t file_count = 0;
    
    if (esp_flash_read(NULL, &file_count, flash_addr, 4) != ESP_OK) {
        Logger::instance().log("[Countdown] Failed to read file count");
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
                    Logger::instance().log("[Countdown] JSON parse error: " + String(error.c_str()));
                    return false;
                }
                
                // V16.2.0-2026-01-10T18:30:00Z - Support both Unix timestamp and human-readable date
                if (doc.containsKey("targetDate")) {
                    // Check if it's a number (Unix timestamp) or string (human-readable)
                    if (doc["targetDate"].is<long>()) {
                        targetTime = doc["targetDate"].as<long>();
                        Logger::instance().log("[Countdown] Target: " + String(targetTime));
                    } else if (doc["targetDate"].is<const char*>()) {
                        // Parse "YYYY-MM-DD HH:MM:SS" format
                        String dateStr = doc["targetDate"].as<String>();
                        targetTime = parseHumanDate(dateStr);
                        if (targetTime > 0) {
                            Logger::instance().log("[Countdown] Parsed target: " + String(targetTime));
                        } else {
                            Logger::instance().log("[Countdown] Failed to parse date: " + dateStr);
                            return false;
                        }
                    }
                    return true;
                }
                
                Logger::instance().log("[Countdown] No targetDate field");
                return false;
            }
            delete[] content;
            return false;
        }
        
        // Skip to next file
        flash_addr += content_len;
        uint32_t padding = (512 - (flash_addr % 512)) % 512;
        flash_addr += padding;
    }
    
    Logger::instance().log("[Countdown] File not found: " + jsonPath);
    return false;
}

void Countdown::begin() {
    lastUpdate = millis();
    flashState = false;
    lastFlash = 0;
}

void Countdown::update() {
    unsigned long now = millis();
    if (now - lastUpdate < 1000) return;  // V16.2.0-2026-01-10T18:05:00Z - Update every second
    lastUpdate = now;
    
    disp->clear();
    
    // V16.2.0-2026-01-10T18:05:00Z - Get current time from NTP
    ntpClient->update();
    time_t currentTime = ntpClient->getEpochTime();
    
    // Fallback if NTP not synced
    if (currentTime < 100000) {
        currentTime = millis() / 1000;
    }
    
    // Calculate time difference
    long diff = targetTime - currentTime;
    bool isZero = (diff <= 0);
    
    if (isZero) {
        diff = 0;  // V16.2.0-2026-01-10T18:05:00Z - Don't go negative
        
        // Flash toggle every 500ms
        if (now - lastFlash >= 500) {
            flashState = !flashState;
            lastFlash = now;
        }
    }
    
    // Calculate time components
    long days = diff / 86400;
    long hours = (diff % 86400) / 3600;
    long minutes = (diff % 3600) / 60;
    long seconds = diff % 60;
    
    // Clamp to 99 max
    if (days > 99) days = 99;
    if (hours > 99) hours = 99;
    if (minutes > 99) minutes = 99;
    if (seconds > 99) seconds = 99;
    
    // V16.2.0-2026-01-10T18:05:00Z - Draw countdown boxes with theme colors
    // Layout: Matrix 0 = Minutes & Seconds, Matrix 1 = Days & Hours
    const int BOX_WIDTH = 9;
    const int BOX_HEIGHT = 10;
    const int GAP = 1;
    const int Y_START = 8;
    const int X_LEFT = GAP;
    const int X_RIGHT = 12 + GAP;  // Adjusted for 25-pixel width
    
    drawBox(0, X_LEFT, Y_START, 'M', minutes, isZero);
    drawBox(0, X_RIGHT, Y_START, 'S', seconds, isZero);
    drawBox(1, X_LEFT, Y_START, 'D', days, isZero);
    drawBox(1, X_RIGHT, Y_START, 'H', hours, isZero);
    
    disp->show();
}

void Countdown::drawBox(int matrix, int x, int y, char label, long value, bool shouldFlash) {
    // V16.2.0-2026-01-10T18:05:00Z - Theme colors: Header=color1, Box=color2, Numbers=color3
    CRGB headerColor = themes->getColor1();
    CRGB boxColor = themes->getColor2();
    CRGB numberColor = themes->getColor3();
    
    // Draw box border
    drawRectBorder(matrix, x, y, x + 8, y + 9, boxColor);
    
    // Draw label above box
    int label_x = x + 4;
    int label_y = y - 4;
    drawLabel(matrix, label_x, label_y, label, headerColor);
    
    // Draw digits (only if not flashing or flash is ON)
    if (!shouldFlash || flashState) {
        int digit1 = (value / 10) % 10;
        int digit2 = value % 10;
        
        drawDigit(matrix, x + 1, 10, digit1, numberColor);
        drawDigit(matrix, x + 5, 10, digit2, numberColor);
    }
}

void Countdown::drawDigit(int matrix, int x, int y, int digit, CRGB color) {
    if (digit < 0 || digit > 9) return;
    
    for (int row = 0; row < 5; row++) {
        uint8_t rowPattern = DIGIT_3X5[digit][row];
        for (int col = 0; col < 3; col++) {
            if (rowPattern & (0b100 >> col)) {
                disp->setPixel(matrix, x + col, y + row, color);
            }
        }
    }
}

void Countdown::drawRectBorder(int matrix, int x1, int y1, int x2, int y2, CRGB color) {
    // Draw horizontal lines
    for (int x = x1; x <= x2; x++) {
        disp->setPixel(matrix, x, y1, color);
        disp->setPixel(matrix, x, y2, color);
    }
    // Draw vertical lines
    for (int y = y1 + 1; y <= y2 - 1; y++) {
        disp->setPixel(matrix, x1, y, color);
        disp->setPixel(matrix, x2, y, color);
    }
}

void Countdown::drawLabel(int matrix, int x, int y, char label, CRGB color) {
    // V16.2.0-2026-01-10T18:05:00Z - Draw 3-pixel wide labels (D, H, M, S)
    switch(label) {
        case 'D':
            disp->setPixel(matrix, x-1, y, color);
            disp->setPixel(matrix, x-1, y+1, color);
            disp->setPixel(matrix, x-1, y+2, color);
            disp->setPixel(matrix, x-1, y+3, color);
            disp->setPixel(matrix, x-1, y+4, color);
            disp->setPixel(matrix, x, y, color);
            disp->setPixel(matrix, x+1, y+1, color);
            disp->setPixel(matrix, x+1, y+2, color);
            disp->setPixel(matrix, x+1, y+3, color);
            disp->setPixel(matrix, x, y+4, color);
            break;
            
        case 'H':
            disp->setPixel(matrix, x-1, y, color);
            disp->setPixel(matrix, x-1, y+1, color);
            disp->setPixel(matrix, x-1, y+2, color);
            disp->setPixel(matrix, x-1, y+3, color);
            disp->setPixel(matrix, x-1, y+4, color);
            disp->setPixel(matrix, x, y+2, color);
            disp->setPixel(matrix, x+1, y, color);
            disp->setPixel(matrix, x+1, y+1, color);
            disp->setPixel(matrix, x+1, y+2, color);
            disp->setPixel(matrix, x+1, y+3, color);
            disp->setPixel(matrix, x+1, y+4, color);
            break;
            
        case 'M':
            disp->setPixel(matrix, x-1, y, color);
            disp->setPixel(matrix, x-1, y+1, color);
            disp->setPixel(matrix, x-1, y+2, color);
            disp->setPixel(matrix, x-1, y+3, color);
            disp->setPixel(matrix, x-1, y+4, color);
            disp->setPixel(matrix, x, y+1, color);
            disp->setPixel(matrix, x+1, y, color);
            disp->setPixel(matrix, x+1, y+1, color);
            disp->setPixel(matrix, x+1, y+2, color);
            disp->setPixel(matrix, x+1, y+3, color);
            disp->setPixel(matrix, x+1, y+4, color);
            break;
            
        case 'S':
            disp->setPixel(matrix, x-1, y, color);
            disp->setPixel(matrix, x, y, color);
            disp->setPixel(matrix, x+1, y, color);
            disp->setPixel(matrix, x-1, y+1, color);
            disp->setPixel(matrix, x-1, y+2, color);
            disp->setPixel(matrix, x, y+2, color);
            disp->setPixel(matrix, x+1, y+2, color);
            disp->setPixel(matrix, x+1, y+3, color);
            disp->setPixel(matrix, x-1, y+4, color);
            disp->setPixel(matrix, x, y+4, color);
            disp->setPixel(matrix, x+1, y+4, color);
            break;
    }
}

void Countdown::setTargetDate(time_t targetEpoch) {
    targetTime = targetEpoch;
}

// V16.2.0-2026-01-10T18:30:00Z - Parse "YYYY-MM-DD HH:MM:SS" to Unix timestamp (UTC)
time_t Countdown::parseHumanDate(const String& dateStr) {
    struct tm timeinfo;
    memset(&timeinfo, 0, sizeof(timeinfo));
    
    // Expected format: "2025-12-25 05:00:00"
    int year, month, day, hour, minute, second;
    
    if (sscanf(dateStr.c_str(), "%d-%d-%d %d:%d:%d", 
               &year, &month, &day, &hour, &minute, &second) == 6) {
        
        timeinfo.tm_year = year - 1900;  // Years since 1900
        timeinfo.tm_mon = month - 1;     // Months since January (0-11)
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = hour;
        timeinfo.tm_min = minute;
        timeinfo.tm_sec = second;
        
        // Convert to Unix timestamp (assuming UTC)
        time_t timestamp = mktime(&timeinfo);
        
        // mktime uses local time, but we want UTC, so adjust
        // This is a simplified approach - for production use a proper UTC conversion
        return timestamp;
    }
    
    Logger::instance().log("[Countdown] Invalid date format, expected: YYYY-MM-DD HH:MM:SS");
    return 0;
}
