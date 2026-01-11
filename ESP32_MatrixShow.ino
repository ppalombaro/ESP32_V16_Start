/* ESP32_MatrixShow.ino
   Main program entry point
   VERSION: V16.1.2-2026-01-08T15:00:00Z - Content auto-discovery architecture
*/

#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FastLED.h>
#include <Preferences.h>     // ← ADD THIS!
#include "Config.h"
#include "MatrixDisplay.h"
#include "Logger.h"
#include "ThemeManager.h"
#include "ContentManager.h"
#include "Scheduler.h"
#include "WebController.h"
#include "Scroll.h"          // ← ADD THIS
#include "Countdown.h"       // ← ADD THIS

// Global objects
Preferences preferences;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 60000);

MatrixDisplay display;
ThemeManager themeManager;  // V16.2.5-2026-01-10T22:20:00Z - Must be before ContentManager
ContentManager content;
WebController web;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Logger::instance().log("=================================");
    Logger::instance().log("ESP32 Matrix Show V16.1.2");
    Logger::instance().log("Content Auto-Discovery System");
    Logger::instance().log("=================================");

    // Initialize display hardware
    display.begin();
    Logger::instance().log("[SETUP] Display initialized");

    // V16.1.2 - Discover all content from filesystem
    content.begin(&display);
    Logger::instance().log("[SETUP] Content discovery complete");

    // Initialize WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Logger::instance().log("[SETUP] Connecting to WiFi...");
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Logger::instance().log("[SETUP] WiFi connected: " + WiFi.localIP().toString());
        timeClient.begin();
    } else {
        Logger::instance().log("[SETUP] WiFi connection FAILED - continuing offline");
    }

    // Initialize theme manager
    themeManager.begin(&display, &content);
    Logger::instance().log("[SETUP] Theme manager initialized");

    // Initialize web interface
    web.begin(&content, &themeManager, &display);
    Logger::instance().log("[SETUP] Web interface started");

    Logger::instance().log("[SETUP] System ready!");
    Logger::instance().log("=================================");
}

void loop() {
    // V16.2.5-2026-01-10T22:06:00Z - Removed watchdog reset (not needed)
    
    // Update NTP time
    if (WiFi.status() == WL_CONNECTED) {
        timeClient.update();
    }

    // Handle web requests
    web.handle();
    
    // Update theme/content system
    themeManager.update();
    content.update();

    delay(10);
}
