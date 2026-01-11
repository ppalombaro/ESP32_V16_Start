/* WebActions.cpp
   API endpoints for web interface
   VERSION: V16.1.3-2026-01-09T05:25:00Z - Added brightness control with NVS
*/

#include "WebActions.h"
#include "ContentManager.h"
#include "ThemeManager.h"
#include "MatrixDisplay.h"
#include "Logger.h"
#include <WebServer.h>
#include <Preferences.h>

static Preferences prefs;

WebActions::WebActions(ContentManager* cm, ThemeManager* tm, MatrixDisplay* disp, WebServer* srv)
: contentMgr(cm), themeMgr(tm), display(disp), server(srv) {}

void WebActions::attach() {
    
    // Content rendering
    server->on("/api/render", HTTP_GET, [this]() {
        if (!server->hasArg("id")) {
            server->send(400, "text/plain", "Missing 'id' parameter");
            return;
        }
        
        uint16_t id = server->arg("id").toInt();
        bool success = contentMgr->renderContent(id);
        
        const ContentItem* item = contentMgr->getContentById(id);
        String response = success ? "âœ… Rendering: " + (item ? item->name : "Unknown") : "âŒ Content not found";
        server->send(success ? 200 : 404, "text/plain", response);
    });
    
    // Clear display
    server->on("/api/clear", HTTP_GET, [this]() {
        if (display) {
            display->clear();
            display->show();
        }
        Logger::instance().log("[WebActions] Display cleared");
        server->send(200, "text/plain", "âœ… Display cleared");
    });
    
    // Test pattern
    server->on("/api/test", HTTP_GET, [this]() {
        Logger::instance().log("[WebActions] Test pattern activated");
        server->send(200, "text/plain", "âœ… Test pattern activated");
    });
    
    // Random mode control
    server->on("/api/random/enable", HTTP_GET, [this]() {
        contentMgr->enableRandomMode(true);
        server->send(200, "text/plain", "âœ… Random mode ENABLED");
    });
    
    server->on("/api/random/disable", HTTP_GET, [this]() {
        contentMgr->enableRandomMode(false);
        server->send(200, "text/plain", "â›” Random mode DISABLED");
    });
    
    server->on("/api/random/interval", HTTP_GET, [this]() {
        if (!server->hasArg("ms")) {
            server->send(400, "text/plain", "Missing 'ms' parameter");
            return;
        }
        
        unsigned long intervalMs = server->arg("ms").toInt();
        contentMgr->setRandomInterval(intervalMs);
        server->send(200, "text/plain", "âœ… Interval set to " + String(intervalMs) + " ms");
    });
    
    server->on("/api/random/filter", HTTP_GET, [this]() {
        if (!server->hasArg("theme")) {
            server->send(400, "text/plain", "Missing 'theme' parameter");
            return;
        }
        
        String theme = server->arg("theme");
        contentMgr->setRandomThemeFilter(theme);
        
        String response = theme.length() > 0 ? 
                         "âœ… Filter set to: " + theme : 
                         "âœ… Filter cleared (ALL THEMES)";
        server->send(200, "text/plain", response);
    });
    
    // Scheduler control
    server->on("/api/scheduler/enable", HTTP_GET, [this]() {
        contentMgr->enableScheduler(true);
        server->send(200, "text/plain", "âœ… Scheduler ENABLED");
    });
    
    server->on("/api/scheduler/disable", HTTP_GET, [this]() {
        contentMgr->enableScheduler(false);
        server->send(200, "text/plain", "â›” Scheduler DISABLED");
    });
    
    // Brightness control
    server->on("/api/brightness", HTTP_GET, [this]() {
        if (!server->hasArg("value")) {
            server->send(400, "text/plain", "Missing 'value' parameter");
            return;
        }
        
        uint8_t brightness = server->arg("value").toInt();
        if (brightness < 1) brightness = 1;
        if (brightness > 255) brightness = 255;
        
        if (display) {
            display->setBrightness(brightness);
        }
        
        // Save to NVS
        saveBrightness(brightness);
        
        Logger::instance().log("[WebActions] Brightness set to " + String(brightness));
        server->send(200, "text/plain", String(brightness));
    });
    
    server->on("/api/brightness/get", HTTP_GET, [this]() {
        uint8_t brightness = loadBrightness();
        server->send(200, "text/plain", String(brightness));
    });
    
    // Logs
    server->on("/api/logs/clear", HTTP_GET, [this]() {
        Logger::instance().clear();
        server->send(200, "text/plain", "âœ… Logs cleared");
    });
    
    // Theme control (legacy)
    server->on("/api/theme/set", HTTP_GET, [this]() {
        if (!server->hasArg("id")) {
            server->send(400, "text/plain", "Missing 'id' parameter");
            return;
        }
        
        uint8_t themeId = server->arg("id").toInt();
        themeMgr->setTheme(themeId);
        server->send(200, "text/plain", "âœ… Theme set to " + String(themeId));
    });
}

void WebActions::saveBrightness(uint8_t brightness) {
    prefs.begin("matrixshow", false);
    prefs.putUChar("brightness", brightness);
    prefs.end();
}

uint8_t WebActions::loadBrightness() {
    prefs.begin("matrixshow", true);
    uint8_t brightness = prefs.getUChar("brightness", 20);  // Default 20
    prefs.end();
    return brightness;
}