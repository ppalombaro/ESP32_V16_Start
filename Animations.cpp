/* Animations.cpp
   Procedural animations implementation
   VERSION: V16.2.3-2026-01-10T21:40:00Z - Namespace-based (NO CLASS)
*/

#include "Animations.h"
#include <FastLED.h>

// V16.2.3-2026-01-10T21:40:00Z - Procedural animations in namespace (NOT a class!)

namespace Animations {

// Chase animation - diagonal lines bouncing
void chase(MatrixDisplay* disp) {
    static int position = 0;
    static int direction = 1;
    static int colorIndex = 0;
    static unsigned long lastUpdate = 0;
    
    unsigned long now = millis();
    if (now - lastUpdate < 50) return;
    lastUpdate = now;
    
    disp->clear();
    
    CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Cyan, CRGB::White};
    
    int spacing = 8;
    // V16.2.4-2026-01-10T21:52:00Z - Use Config.h COLS/ROWS macros
    
    for (int matrix = 0; matrix < 2; matrix++) {
        for (int lineNum = 0; lineNum < 3; lineNum++) {
            int linePosition = position + (lineNum * spacing);
            CRGB lineColor = colors[(colorIndex + lineNum) % 4];
            
            for (int offset = -ROWS; offset < COLS; offset++) {
                if (offset == linePosition) {
                    for (int i = 0; i < ROWS; i++) {
                        int x = offset + i;
                        int y = i;
                        if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
                            disp->setPixel(matrix, x, y, lineColor);
                        }
                    }
                }
            }
        }
    }
    
    position += direction;
    if (position >= COLS - 1) {
        position = COLS - 1;
        direction = -1;
        colorIndex++;
    } else if (position <= -(ROWS - 1)) {
        position = -(ROWS - 1);
        direction = 1;
        colorIndex++;
    }
    
    disp->show();
}

// Snowfall animation - standard speed
void snowfall(MatrixDisplay* disp) {
    // V16.2.4-2026-01-10T21:52:00Z - Use Config.h COLS/ROWS macros
    const int MAX_FLAKES = 50;
    const CRGB SNOW_WHITE = CRGB(220, 240, 255);
    
    static struct Flake {
        float x, y, dx, dy;
    } flakes[2][MAX_FLAKES];
    
    static bool initialized = false;
    static unsigned long lastUpdate = 0;
    
    if (!initialized) {
        for (int m = 0; m < 2; m++) {
            for (int i = 0; i < MAX_FLAKES; i++) {
                flakes[m][i].x = random(COLS);
                flakes[m][i].y = random(ROWS);
                flakes[m][i].dx = (random(200) - 100) / 100.0f;
                flakes[m][i].dy = (random(50) + 50) / 100.0f;
            }
        }
        initialized = true;
    }
    
    unsigned long now = millis();
    if (now - lastUpdate < 50) return;
    lastUpdate = now;
    
    disp->clear();
    
    for (int m = 0; m < 2; m++) {
        for (int i = 0; i < MAX_FLAKES; i++) {
            flakes[m][i].x += flakes[m][i].dx;
            flakes[m][i].y -= flakes[m][i].dy;
            
            if (flakes[m][i].x < 0) flakes[m][i].x = COLS - 1;
            if (flakes[m][i].x >= COLS) flakes[m][i].x = 0;
            if (flakes[m][i].y < 0) {
                flakes[m][i].y = ROWS - 1;
                flakes[m][i].x = random(COLS);
            }
            
            int x = (int)flakes[m][i].x;
            int y = (int)flakes[m][i].y;
            if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
                disp->setPixel(m, x, y, SNOW_WHITE);
            }
        }
    }
    
    disp->show();
}

// Gentle snowfall - slower
void snowfallGentle(MatrixDisplay* disp) {
    // V16.2.4-2026-01-10T21:52:00Z - Use Config.h COLS/ROWS macros
    const int MAX_FLAKES = 30;
    const CRGB SNOW_WHITE = CRGB(220, 240, 255);
    
    static struct Flake {
        float x, y, dx, dy;
    } flakes[2][MAX_FLAKES];
    
    static bool initialized = false;
    static unsigned long lastUpdate = 0;
    
    if (!initialized) {
        for (int m = 0; m < 2; m++) {
            for (int i = 0; i < MAX_FLAKES; i++) {
                flakes[m][i].x = random(COLS);
                flakes[m][i].y = random(ROWS);
                flakes[m][i].dx = (random(100) - 50) / 100.0f;
                flakes[m][i].dy = (random(30) + 20) / 100.0f;
            }
        }
        initialized = true;
    }
    
    unsigned long now = millis();
    if (now - lastUpdate < 80) return;
    lastUpdate = now;
    
    disp->clear();
    
    for (int m = 0; m < 2; m++) {
        for (int i = 0; i < MAX_FLAKES; i++) {
            flakes[m][i].x += flakes[m][i].dx;
            flakes[m][i].y -= flakes[m][i].dy;
            
            if (flakes[m][i].x < 0) flakes[m][i].x = COLS - 1;
            if (flakes[m][i].x >= COLS) flakes[m][i].x = 0;
            if (flakes[m][i].y < 0) {
                flakes[m][i].y = ROWS - 1;
                flakes[m][i].x = random(COLS);
            }
            
            int x = (int)flakes[m][i].x;
            int y = (int)flakes[m][i].y;
            if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
                disp->setPixel(m, x, y, SNOW_WHITE);
            }
        }
    }
    
    disp->show();
}

// Heavy snowfall - faster
void snowfallHeavy(MatrixDisplay* disp) {
    // V16.2.4-2026-01-10T21:52:00Z - Use Config.h COLS/ROWS macros
    const int MAX_FLAKES = 80;
    const CRGB SNOW_WHITE = CRGB(220, 240, 255);
    
    static struct Flake {
        float x, y, dx, dy;
    } flakes[2][MAX_FLAKES];
    
    static bool initialized = false;
    static unsigned long lastUpdate = 0;
    
    if (!initialized) {
        for (int m = 0; m < 2; m++) {
            for (int i = 0; i < MAX_FLAKES; i++) {
                flakes[m][i].x = random(COLS);
                flakes[m][i].y = random(ROWS);
                flakes[m][i].dx = (random(300) - 150) / 100.0f;
                flakes[m][i].dy = (random(80) + 80) / 100.0f;
            }
        }
        initialized = true;
    }
    
    unsigned long now = millis();
    if (now - lastUpdate < 30) return;
    lastUpdate = now;
    
    disp->clear();
    
    for (int m = 0; m < 2; m++) {
        for (int i = 0; i < MAX_FLAKES; i++) {
            flakes[m][i].x += flakes[m][i].dx;
            flakes[m][i].y -= flakes[m][i].dy;
            
            if (flakes[m][i].x < 0) flakes[m][i].x = COLS - 1;
            if (flakes[m][i].x >= COLS) flakes[m][i].x = 0;
            if (flakes[m][i].y < 0) {
                flakes[m][i].y = ROWS - 1;
                flakes[m][i].x = random(COLS);
            }
            
            int x = (int)flakes[m][i].x;
            int y = (int)flakes[m][i].y;
            if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
                disp->setPixel(m, x, y, SNOW_WHITE);
            }
        }
    }
    
    disp->show();
}

// Sparkling stars animation
void sparklingStars(MatrixDisplay* disp) {
    // V16.2.4-2026-01-10T21:52:00Z - Use Config.h COLS/ROWS macros
    static unsigned long lastUpdate = 0;
    
    unsigned long now = millis();
    if (now - lastUpdate < 100) return;
    lastUpdate = now;
    
    disp->clear();
    
    int cx = COLS / 2;
    int cy = ROWS / 2;
    
    for (int matrix = 0; matrix < 2; matrix++) {
        // Main star lines
        for (int i = -8; i <= 8; i++) {
            if (cx + i >= 0 && cx + i < COLS) {
                disp->setPixel(matrix, cx + i, cy, CRGB::Yellow);
            }
            if (cy + i >= 0 && cy + i < ROWS) {
                disp->setPixel(matrix, cx, cy + i, CRGB::Yellow);
            }
        }
        
        // Diagonal lines
        for (int i = -6; i <= 6; i++) {
            if (cx + i >= 0 && cx + i < COLS && cy + i >= 0 && cy + i < ROWS) {
                disp->setPixel(matrix, cx + i, cy + i, CRGB::Yellow);
                disp->setPixel(matrix, cx + i, cy - i, CRGB::Yellow);
            }
        }
        
        // Random sparkles
        for (int i = 0; i < 20; i++) {
            int x = random(COLS);
            int y = random(ROWS);
            if (random(2)) {
                disp->setPixel(matrix, x, y, CRGB::White);
            }
        }
    }
    
    disp->show();
}

} // namespace Animations
