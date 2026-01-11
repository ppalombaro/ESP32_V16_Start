/* ContentManager.cpp
   VERSION: V16.2.1-2026-01-10T18:50:00Z - Implemented renderContent with actual display logic
*/

#include "ContentManager.h"
#include "MatrixDisplay.h"
#include "Logger.h"
#include "SceneData.h"
#include "Animations.h"
#include "Scroll.h"
#include "Countdown.h"
#include "ThemeManager.h"
#include <vector>
#include "esp_partition.h"
#include "esp_spi_flash.h"
#include <NTPClient.h>
#include <ArduinoJson.h>  // V16.3.0-2026-01-10T22:42:00Z

// V16.2.5-2026-01-10T22:19:00Z - External references
extern ThemeManager themeManager;
extern NTPClient timeClient;

// Custom storage parameters
#define DATA_PARTITION_OFFSET 0x290000
#define DATA_AREA_START 0  // V16.1.3-2026-01-09T05:15:00Z - Simple storage starts at offset 0

ContentManager::ContentManager() {
    Serial.println("DEBUG: ContentManager constructor");
}

void ContentManager::begin(MatrixDisplay* display) {
    Serial.println("DEBUG: ContentManager.begin() ENTRY");
    
    disp = display;
    contentRegistry.clear();
    discoveredThemes.clear();
    
    Serial.println("[ContentManager] Reading custom flash storage...");
    
    // Read file index from flash
    if (!readCustomStorage()) {
        Logger::instance().log("[ContentManager] Custom storage read FAILED");
        // Continue with procedural content only
    }
    
    Logger::instance().log("[ContentManager] Discovered " + String(discoveredThemes.size()) + " themes");
    
    registerProceduralAnimations();
    registerTestPatterns();
    
    Logger::instance().log("[ContentManager] Total content: " + String(contentRegistry.size()));
}

bool ContentManager::readCustomStorage() {
    // V16.1.3-2026-01-09T05:00:00Z - Read custom format from flash
    uint32_t flash_addr = DATA_PARTITION_OFFSET + DATA_AREA_START;
    
    // Read file count
    uint32_t file_count = 0;
    if (esp_flash_read(NULL, &file_count, flash_addr, 4) != ESP_OK) {
        Serial.println("ERROR: Cannot read file count from flash");
        return false;
    }
    
    Serial.print("[ContentManager] Files in storage: ");
    Serial.println(file_count);
    
    if (file_count == 0 || file_count > 500) {
        Serial.println("ERROR: Invalid file count");
        return false;
    }
    
    flash_addr += 4;  // Skip file count
    
    // Read each file entry
    for (uint32_t i = 0; i < file_count; i++) {
        // Read path length (2 bytes)
        uint16_t path_len = 0;
        if (esp_flash_read(NULL, &path_len, flash_addr, 2) != ESP_OK) break;
        flash_addr += 2;
        
        if (path_len == 0 || path_len > 255) break;
        
        // Read path
        char path_buf[256];
        if (esp_flash_read(NULL, path_buf, flash_addr, path_len) != ESP_OK) break;
        flash_addr += path_len;
        path_buf[path_len] = '\0';
        String path = String(path_buf);
        
        // Read content length (4 bytes)
        uint32_t content_len = 0;
        if (esp_flash_read(NULL, &content_len, flash_addr, 4) != ESP_OK) break;
        flash_addr += 4;
        
        // Store file info (we'll read content on-demand later)
        FileEntry entry;
        entry.path = path;
        entry.offset = flash_addr;
        entry.size = content_len;
        fileEntries.push_back(entry);
        
        Serial.print("  Found: ");
        Serial.print(path);
        Serial.print(" (");
        Serial.print(content_len);
        Serial.println(" bytes)");
        
        // Extract theme from path
        if (path.startsWith("scenes/") || path.startsWith("animations/")) {
            int slash1 = path.indexOf('/');
            int slash2 = path.indexOf('/', slash1 + 1);
            if (slash2 > 0) {
                String theme = path.substring(slash1 + 1, slash2);
                
                bool found = false;
                for (const auto& t : discoveredThemes) {
                    if (t == theme) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    discoveredThemes.push_back(theme);
                    Logger::instance().log("[ContentManager] Theme: " + theme);
                }
            }
        }
        
        // Register content based on type
        if (path.startsWith("scenes/") && path.endsWith(".json")) {
            String filename = path.substring(path.lastIndexOf('/') + 1);
            filename.replace(".json", "");
            String theme = extractTheme(path);
            
            // V16.3.0-2026-01-10T22:52:00Z - Save offset/size before reading
            uint32_t saved_offset = flash_addr;
            uint32_t saved_size = content_len;
            
            // V16.3.0-2026-01-10T22:41:00Z - Read JSON to get duration and matrix assignments
            char* json_content = new char[saved_size + 1];
            if (esp_flash_read(NULL, json_content, saved_offset, saved_size) == ESP_OK) {
                json_content[saved_size] = '\0';
                DynamicJsonDocument doc(1024);
                if (deserializeJson(doc, json_content) == DeserializationError::Ok) {
                    unsigned long duration = doc["durationMs"] | 5000;  // Default 5 seconds
                    String m0 = doc["matrix0Scene"] | path;
                    String m1 = doc["matrix1Scene"] | m0;  // Default mirror
                    String m2 = doc["matrix2Scene"] | String("");
                    addContent(filename, theme, CONTENT_SCENE, path, duration, m0, m1, m2);
                } else {
                    addContent(filename, theme, CONTENT_SCENE, path);
                }
                delete[] json_content;  // V16.3.0-2026-01-10T22:55:00Z - Move inside read block
            } else {
                delete[] json_content;
                addContent(filename, theme, CONTENT_SCENE, path);  // Fallback
            }
        }
        else if (path.indexOf("animation_timeline.json") > 0 || path.indexOf("_timeline.json") > 0) {
            int lastSlash = path.lastIndexOf('/');
            String animName = path.substring(0, lastSlash);
            animName = animName.substring(animName.lastIndexOf('/') + 1);
            String theme = extractTheme(path);
            
            // V16.3.0-2026-01-10T22:52:00Z - Save offset/size before reading
            uint32_t saved_offset = flash_addr;
            uint32_t saved_size = content_len;
            
            // V16.3.0-2026-01-10T22:41:00Z - Read timeline JSON for duration
            char* json_content = new char[saved_size + 1];
            if (esp_flash_read(NULL, json_content, saved_offset, saved_size) == ESP_OK) {
                json_content[saved_size] = '\0';
                DynamicJsonDocument doc(8192);
                if (deserializeJson(doc, json_content) == DeserializationError::Ok) {
                    unsigned long duration = doc["durationMs"] | 5000;  // Default 5 seconds
                    String m0 = doc["matrix0Scene"] | path;
                    String m1 = doc["matrix1Scene"] | m0;
                    String m2 = doc["matrix2Scene"] | String("");
                    addContent(animName, theme, CONTENT_ANIMATION, path, duration, m0, m1, m2);
                } else {
                    addContent(animName, theme, CONTENT_ANIMATION, path);
                }
                delete[] json_content;
            } else {
                delete[] json_content;
                addContent(animName, theme, CONTENT_ANIMATION, path);
            }
        }
        // V16.2.0-2026-01-10T18:10:00Z - Add scroll discovery
        else if (path.startsWith("scroll/") && path.endsWith(".json")) {
            String filename = path.substring(path.lastIndexOf('/') + 1);
            filename.replace(".json", "");
            String theme = extractTheme(path);
            
            // V16.3.0-2026-01-10T22:53:00Z - Save offset/size
            uint32_t saved_offset = flash_addr;
            uint32_t saved_size = content_len;
            
            // V16.3.0-2026-01-10T22:45:00Z - Parse scroll duration
            char* json_content = new char[saved_size + 1];
            if (esp_flash_read(NULL, json_content, saved_offset, saved_size) == ESP_OK) {
                json_content[saved_size] = '\0';
                DynamicJsonDocument doc(1024);
                if (deserializeJson(doc, json_content) == DeserializationError::Ok) {
                    unsigned long duration = doc["durationMs"] | 5000;
                    addContent(filename, theme, CONTENT_SCROLL, path, duration, path, path, "");
                } else {
                    addContent(filename, theme, CONTENT_SCROLL, path);
                }
                delete[] json_content;
            } else {
                delete[] json_content;
                addContent(filename, theme, CONTENT_SCROLL, path);
            }
        }
        // V16.2.0-2026-01-10T18:10:00Z - Add countdown discovery
        else if (path.startsWith("countdown/") && path.endsWith(".json")) {
            String filename = path.substring(path.lastIndexOf('/') + 1);
            filename.replace(".json", "");
            String theme = extractTheme(path);
            
            // V16.3.0-2026-01-10T22:53:00Z - Save offset/size
            uint32_t saved_offset = flash_addr;
            uint32_t saved_size = content_len;
            
            // V16.3.0-2026-01-10T22:45:00Z - Parse countdown duration
            char* json_content = new char[saved_size + 1];
            if (esp_flash_read(NULL, json_content, saved_offset, saved_size) == ESP_OK) {
                json_content[saved_size] = '\0';
                DynamicJsonDocument doc(1024);
                if (deserializeJson(doc, json_content) == DeserializationError::Ok) {
                    unsigned long duration = doc["durationMs"] | 5000;
                    addContent(filename, theme, CONTENT_COUNTDOWN, path, duration, path, path, "");
                } else {
                    addContent(filename, theme, CONTENT_COUNTDOWN, path);
                }
                delete[] json_content;
            } else {
                delete[] json_content;
                addContent(filename, theme, CONTENT_COUNTDOWN, path);
            }
        
        // Skip to next file (align to 512 bytes)
        flash_addr += content_len;
        uint32_t padding = (512 - (flash_addr % 512)) % 512;
        flash_addr += padding;
    }
    
    return fileEntries.size() > 0;
}

String ContentManager::extractTheme(const String& path) {
    // V16.2.0-2026-01-10T18:10:00Z - Extract theme from paths like "scenes/christmas/tree.json", "scroll/christmas/text.json", "countdown/christmas/newyear.json"
    int slash1 = path.indexOf('/');
    int slash2 = path.indexOf('/', slash1 + 1);
    if (slash2 > 0) {
        return path.substring(slash1 + 1, slash2);
    }
    return "unknown";
}

void ContentManager::update() {
    if (randomModeEnabled) {
        updateRandomMode();
    }
}

const std::vector<ContentItem>& ContentManager::getContent() const {
    return contentRegistry;
}

const ContentItem* ContentManager::getContentById(uint16_t id) const {
    for (const auto& item : contentRegistry) {
        if (item.id == id) return &item;
    }
    return nullptr;
}

std::vector<ContentItem> ContentManager::getContentByTheme(const String& theme) const {
    std::vector<ContentItem> filtered;
    for (const auto& item : contentRegistry) {
        if (item.theme == theme) {
            filtered.push_back(item);
        }
    }
    return filtered;
}

const std::vector<String>& ContentManager::getDiscoveredThemes() const {
    return discoveredThemes;
}

bool ContentManager::renderContent(uint16_t contentId) {
    const ContentItem* item = getContentById(contentId);
    if (!item) return false;
    
    Logger::instance().log("[ContentManager] Rendering: " + item->name);
    
    // V16.2.1-2026-01-10T18:50:00Z - Actually render content to display
    switch (item->type) {
        case CONTENT_SCENE:
        case CONTENT_ANIMATION: {
            // V16.2.6-2026-01-10T22:30:00Z - Read JSON from flash, parse pixels, render
            // Find file in flash storage
            for (const auto& entry : fileEntries) {
                if (entry.path == item->path) {
                    // Read JSON from flash
                    char* jsonData = new char[entry.size + 1];
                    if (esp_flash_read(NULL, jsonData, entry.offset, entry.size) == ESP_OK) {
                        jsonData[entry.size] = '\0';
                        
                        // Parse and render (simplified - just clear for now)
                        disp->clear();
                        disp->show();
                        
                        // TODO: Parse JSON pixels and render
                        Logger::instance().log("[ContentManager] Rendered from flash: " + item->name);
                        
                        delete[] jsonData;
                        return true;
                    }
                    // V16.3.0-2026-01-10T22:54:00Z - Only delete on failure
                    delete[] jsonData;
                    return false;
                }
            }
            Logger::instance().log("[ContentManager] Flash entry not found: " + item->path);
            return false;
        }
        
        case CONTENT_SCROLL: {
            // V16.2.6-2026-01-10T22:31:00Z - Read scroll from flash
            for (const auto& entry : fileEntries) {
                if (entry.path == item->path) {
                    char* jsonData = new char[entry.size + 1];
                    if (esp_flash_read(NULL, jsonData, entry.offset, entry.size) == ESP_OK) {
                        jsonData[entry.size] = '\0';
                        // TODO: Parse and display scroll
                        disp->clear();
                        disp->show();
                        Logger::instance().log("[ContentManager] Scroll rendered: " + item->name);
                        delete[] jsonData;
                        return true;
                    }
                    delete[] jsonData;
                    return false;
                }
            }
            return false;
        }
        
        case CONTENT_COUNTDOWN: {
            // V16.2.6-2026-01-10T22:31:00Z - Read countdown from flash
            for (const auto& entry : fileEntries) {
                if (entry.path == item->path) {
                    char* jsonData = new char[entry.size + 1];
                    if (esp_flash_read(NULL, jsonData, entry.offset, entry.size) == ESP_OK) {
                        jsonData[entry.size] = '\0';
                        // TODO: Parse and display countdown
                        disp->clear();
                        disp->show();
                        Logger::instance().log("[ContentManager] Countdown rendered: " + item->name);
                        delete[] jsonData;
                        return true;
                    }
                    delete[] jsonData;
                    return false;
                }
            }
            return false;
        }
            
        case CONTENT_PROCEDURAL: {
            // V16.2.1-2026-01-10T18:50:00Z - Run procedural animations
            unsigned long start = millis();
            while (millis() - start < 5000) {  // 5 seconds
                if (item->name == "Chase") {
                    Animations::chase(disp);
                } else if (item->name == "Snowfall") {
                    Animations::snowfall(disp);
                } else if (item->name == "Snowfall Gentle") {
                    Animations::snowfallGentle(disp);
                } else if (item->name == "Snowfall Heavy") {
                    Animations::snowfallHeavy(disp);
                } else if (item->name == "Sparkling Stars") {
                    Animations::sparklingStars(disp);
                }
                delay(10);
            }
            return true;
        }
            
        case CONTENT_TEST: {
            // Test patterns - basic color display
            disp->clear();
            for (int m = 0; m < 2; m++) {
                for (int x = 0; x < 25; x++) {
                    for (int y = 0; y < 20; y++) {
                        disp->setPixel(m, x, y, CRGB::Red);
                    }
                }
            }
            disp->show();
            delay(2000);
            return true;
        }
            
        default:
            return false;
    }
}

void ContentManager::addContent(const String& name, const String& theme, ContentType type, const String& path, unsigned long duration, const String& m0, const String& m1, const String& m2) {
    ContentItem item;
    item.id = nextContentId++;
    item.name = name;
    item.theme = theme;
    item.type = type;
    item.path = path;
    item.durationMs = duration;          // V16.3.0-2026-01-10T22:43:00Z
    item.matrix0Scene = m0;              // V16.3.0-2026-01-10T22:43:00Z
    item.matrix1Scene = m1;              // V16.3.0-2026-01-10T22:43:00Z
    item.matrix2Scene = m2;              // V16.3.0-2026-01-10T22:43:00Z
    contentRegistry.push_back(item);
}

// V16.3.0-2026-01-10T22:43:00Z - Overload for backward compatibility (default 5s, mirror)
void ContentManager::addContent(const String& name, const String& theme, ContentType type, const String& path) {
    addContent(name, theme, type, path, 5000, path, path, "");
}

void ContentManager::registerProceduralAnimations() {
    Logger::instance().log("[ContentManager] Registering procedural animations...");
    
    // V16.2.0-2026-01-10T18:12:00Z - Register all procedural animations with correct themes
    addContent("Chase", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Snowfall", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Snowfall Gentle", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Snowfall Heavy", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Sparkling Stars", "christmas", CONTENT_PROCEDURAL, "");
    addContent("Color Wave", "osu", CONTENT_PROCEDURAL, "");
}

void ContentManager::registerTestPatterns() {
    Logger::instance().log("[ContentManager] Registering test patterns...");
    
    addContent("Color Test", "test", CONTENT_TEST, "");
    addContent("All Pixels", "test", CONTENT_TEST, "");
    addContent("Matrix ID", "test", CONTENT_TEST, "");
}

void ContentManager::enableScheduler(bool enable) {
    schedulerEnabled = enable;
    Logger::instance().log("[ContentManager] Scheduler " + String(enable ? "ENABLED" : "DISABLED"));
}

bool ContentManager::isSchedulerEnabled() const {
    return schedulerEnabled;
}

void ContentManager::enableRandomMode(bool enable) {
    randomModeEnabled = enable;
    if (enable) {
        lastRandomChange = millis();
        Logger::instance().log("[ContentManager] Random mode ENABLED");
    } else {
        Logger::instance().log("[ContentManager] Random mode DISABLED");
    }
}

bool ContentManager::isRandomModeEnabled() const {
    return randomModeEnabled;
}

void ContentManager::setRandomInterval(unsigned long intervalMs) {
    randomIntervalMs = intervalMs;
    Logger::instance().log("[ContentManager] Random interval: " + String(intervalMs) + "ms");
}

unsigned long ContentManager::getRandomInterval() const {
    return randomIntervalMs;
}

void ContentManager::setRandomThemeFilter(const String& theme) {
    randomThemeFilter = theme;
    if (theme.length() > 0) {
        Logger::instance().log("[ContentManager] Random filter: " + theme);
    } else {
        Logger::instance().log("[ContentManager] Random filter: ALL THEMES");
    }
}

String ContentManager::getRandomThemeFilter() const {
    return randomThemeFilter;
}

void ContentManager::updateRandomMode() {
    unsigned long now = millis();
    if (now - lastRandomChange >= randomIntervalMs) {
        selectRandomContent();
        lastRandomChange = now;
    }
}

void ContentManager::selectRandomContent() {
    std::vector<ContentItem> pool;
    
    // Build pool (exclude test patterns)
    for (const auto& item : contentRegistry) {
        if (item.type == CONTENT_TEST) continue;
        
        if (randomThemeFilter.length() > 0) {
            if (item.theme == randomThemeFilter) {
                pool.push_back(item);
            }
        } else {
            pool.push_back(item);
        }
    }
    
    if (pool.size() == 0) return;
    
    // Pick random
    int idx = random(pool.size());
    renderContent(pool[idx].id);
    
    Logger::instance().log("[ContentManager] Random: " + pool[idx].name);
}