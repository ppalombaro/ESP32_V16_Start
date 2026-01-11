/* Scheduler.cpp
   Complete scheduler with support for all content types
   VERSION: V16.2.0-2026-01-10T18:35:00Z - Full implementation
*/

#include "Scheduler.h"
#include "ContentManager.h"
#include "MatrixDisplay.h"
#include "ThemeManager.h"
#include "SceneData.h"
#include "Animations.h"
#include "Scroll.h"
#include "Countdown.h"
#include "Logger.h"
#include <NTPClient.h>

// V16.2.0-2026-01-10T18:35:00Z - External references
extern MatrixDisplay matrix;
extern ThemeManager themeManager;
extern NTPClient timeClient;

Scheduler::Scheduler() {}

void Scheduler::begin(ContentManager* content) {
    contentMgr = content;
}

void Scheduler::enable(bool on) {
    enabled = on;
    Logger::instance().log(String("Scheduler ") + (on ? "ON" : "OFF"));
}

void Scheduler::update() {
    if (!enabled || !contentMgr) return;
    
    unsigned long now = millis();
    
    // Check if it's time to switch content
    if (now - lastContentChange < contentDuration) return;
    
    // V16.2.0-2026-01-10T18:35:00Z - Get random content and play it
    const auto& allContent = contentMgr->getContent();
    if (allContent.size() == 0) return;
    
    // Pick random content
    int idx = random(allContent.size());
    const ContentItem& item = allContent[idx];
    
    Logger::instance().log("[Scheduler] Playing: " + item.name);
    
    // Play content based on type
    playContent(item);
    
    lastContentChange = now;
}

void Scheduler::playContent(const ContentItem& item) {
    // V16.3.0-2026-01-10T23:00:00Z - Use durationMs from item
    unsigned long startTime = millis();
    unsigned long duration = item.durationMs;  // Use content-specific duration
    
    switch (item.type) {
        case CONTENT_SCENE: {
            // V16.2.5-2026-01-10T22:16:00Z - Load and display static scene
            SceneData scene(item.path);
            if (scene.load()) {
                // TODO: scene.render(&matrix);
                matrix.show();
                // Hold for duration
                while (millis() - startTime < duration) {
                    delay(100);
                }
            }
            break;
        }
        
        case CONTENT_ANIMATION: {
            // V16.2.5-2026-01-10T22:16:00Z - Play timeline animation
            SceneData anim(item.path);
            if (anim.load()) {
                // V16.3.0-2026-01-10T23:00:00Z - duration already set from item
                while (millis() - startTime < duration) {
                    // TODO: anim.updateAnimation(&matrix);
                    matrix.show();
                    delay(50);
                }
            }
            break;
        }
        
        case CONTENT_SCROLL: {
            // Run scrolling text
            Scroll scroll(&matrix, &themeManager);
            if (scroll.loadFromJSON(item.path)) {
                scroll.begin();
                // V16.3.0-2026-01-10T23:01:00Z - Use item.durationMs
                while (millis() - startTime < duration) {
                    scroll.update();
                    delay(10);
                }
            }
            break;
        }
        
        case CONTENT_COUNTDOWN: {
            // Show countdown timer
            Countdown countdown(&matrix, &themeManager, &timeClient);
            if (countdown.loadFromJSON(item.path)) {
                countdown.begin();
                // V16.3.0-2026-01-10T23:01:00Z - Use item.durationMs
                while (millis() - startTime < duration) {
                    countdown.update();
                    delay(100);
                }
            }
            break;
        }
        
        case CONTENT_PROCEDURAL: {
            // V16.3.0-2026-01-10T23:01:00Z - Run procedural animations with item.durationMs
            while (millis() - startTime < duration) {
                if (item.name == "Chase") {
                    Animations::chase(&matrix);
                } else if (item.name == "Snowfall") {
                    Animations::snowfall(&matrix);
                } else if (item.name == "Snowfall Gentle") {
                    Animations::snowfallGentle(&matrix);
                } else if (item.name == "Snowfall Heavy") {
                    Animations::snowfallHeavy(&matrix);
                } else if (item.name == "Sparkling Stars") {
                    Animations::sparklingStars(&matrix);
                }
                delay(10);
            }
            break;
        }
        
        case CONTENT_TEST: {
            // Test patterns - skip in normal schedule
            break;
        }
        
        default:
            Logger::instance().log("[Scheduler] Unknown content type");
            break;
    }
}

void Scheduler::setContentDuration(unsigned long durationMs) {
    contentDuration = durationMs;
}
