/* ContentManager.h
   Content discovery and rendering system
   VERSION: V16.2.5-2026-01-10T22:05:00Z - Fixed header to match .cpp implementation
*/

#pragma once

#include <Arduino.h>
#include <vector>

// V16.2.5-2026-01-10T22:05:00Z - Forward declarations
class MatrixDisplay;

// V16.2.0 - Content type enumeration
enum ContentType {
    CONTENT_SCENE,
    CONTENT_ANIMATION,
    CONTENT_SCROLL,
    CONTENT_COUNTDOWN,
    CONTENT_PROCEDURAL,  // V16.2.0 - Procedural animations
    CONTENT_TEST         // Test patterns
};

// V16.2.0 - Content item structure
struct ContentItem {
    uint16_t id;
    String name;
    String theme;
    ContentType type;
    String path;
    unsigned long durationMs;     // V16.3.0-2026-01-10T22:40:00Z - Display duration
    String matrix0Scene;          // V16.3.0-2026-01-10T22:40:00Z - Scene for matrix 0
    String matrix1Scene;          // V16.3.0-2026-01-10T22:40:00Z - Scene for matrix 1 (default: mirror matrix0)
    String matrix2Scene;          // V16.3.0-2026-01-10T22:40:00Z - Scene for matrix 2 (optional)
};

// V16.1.3-2026-01-09T05:00:00Z - File entry structure for flash storage
struct FileEntry {
    String path;
    uint32_t offset;
    uint32_t size;
};

class ContentManager {
public:
    ContentManager();
    
    void begin(MatrixDisplay* display);
    void update();
    
    // Content access
    const std::vector<ContentItem>& getContent() const;
    const ContentItem* getContentById(uint16_t id) const;
    std::vector<ContentItem> getContentByTheme(const String& theme) const;
    const std::vector<String>& getDiscoveredThemes() const;
    
    // Content rendering
    bool renderContent(uint16_t contentId);
    
    // Scheduler control
    void enableScheduler(bool enable);
    bool isSchedulerEnabled() const;
    
    // Random mode control
    void enableRandomMode(bool enable);
    bool isRandomModeEnabled() const;
    void setRandomInterval(unsigned long intervalMs);
    unsigned long getRandomInterval() const;
    void setRandomThemeFilter(const String& theme);
    String getRandomThemeFilter() const;

private:
    MatrixDisplay* disp = nullptr;
    
    std::vector<ContentItem> contentRegistry;
    std::vector<String> discoveredThemes;
    std::vector<FileEntry> fileEntries;
    
    uint16_t nextContentId = 1;
    
    bool schedulerEnabled = false;
    bool randomModeEnabled = false;
    unsigned long randomIntervalMs = 10000;
    unsigned long lastRandomChange = 0;
    String randomThemeFilter = "";
    
    // Content registration
    void addContent(const String& name, const String& theme, ContentType type, const String& path, unsigned long duration, const String& m0, const String& m1, const String& m2);
    void addContent(const String& name, const String& theme, ContentType type, const String& path);  // V16.3.0 - Backward compat
    void registerProceduralAnimations();
    void registerTestPatterns();
    
    // Storage reading
    bool readCustomStorage();
    String extractTheme(const String& path);
    
    // Random mode
    void updateRandomMode();
    void selectRandomContent();
};
