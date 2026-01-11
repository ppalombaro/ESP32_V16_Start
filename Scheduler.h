/* Scheduler.h
   Content scheduler with all content type support
   VERSION: V16.2.0-2026-01-10T18:35:00Z - Full implementation
*/

#pragma once
#include <Arduino.h>

// V16.2.4-2026-01-10T21:54:00Z - Forward declarations
class ContentManager;
struct ContentItem;

class Scheduler {
public:
    Scheduler();

    void begin(ContentManager* content);
    void update();
    void enable(bool on);
    
    // V16.2.0-2026-01-10T18:35:00Z - Set how long each content plays
    void setContentDuration(unsigned long durationMs);

private:
    ContentManager* contentMgr = nullptr;
    bool enabled = false;
    unsigned long lastContentChange = 0;
    unsigned long contentDuration = 10000;  // Default 10 seconds
    
    // V16.2.0-2026-01-10T18:35:00Z - Play a single content item
    void playContent(const ContentItem& item);
};
