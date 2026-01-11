/* MatrixDisplay.cpp
   Implementation of display management
   VERSION: V15.2.3-2026-01-04T15:00:00Z - Added matrix dimension getters
   
   V15.2.3-2026-01-04T15:00:00Z - Added getMatrixRows/Cols for per-matrix sizing
   v2.1 - Added drawCircle implementation
   FIXED: Row-major serpentine wiring (bottom-left start, horizontal rows)
*/

#include "MatrixDisplay.h"
#include <Preferences.h>

MatrixDisplay::MatrixDisplay() {}

void MatrixDisplay::begin() {
  FastLED.addLeds<LED_TYPE, PIN_LEFT, COLOR_ORDER>(leds, 0, MATRIX_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, PIN_RIGHT, COLOR_ORDER>(leds, MATRIX_LEDS, MATRIX_LEDS).setCorrection(TypicalLEDStrip);
// V16.1.3-2026-01-09T05:25:00Z - Load saved brightness from NVS
   Preferences prefs;
   prefs.begin("matrixshow", true);
   uint8_t savedBrightness = prefs.getUChar("brightness", DEFAULT_BRIGHTNESS);
   prefs.end();
   FastLED.setBrightness(savedBrightness);
   Serial.printf("Brightness restored: %d\n", savedBrightness);
  FastLED.setMaxRefreshRate(30);
  FastLED.setDither(false);
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
  FastLED.show();
  
  Serial.println("FastLED initialized - Row-major serpentine");
  Serial.printf("Matrix 0 (Left): LEDs 0-%d on GPIO %d\n", MATRIX_LEDS-1, PIN_LEFT);
  Serial.printf("Matrix 1 (Right): LEDs %d-%d on GPIO %d\n", MATRIX_LEDS, TOTAL_LEDS-1, PIN_RIGHT);
}

void MatrixDisplay::show() {
  FastLED.show();
}

void MatrixDisplay::clear() {
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);
}

void MatrixDisplay::clearMatrix(int matrix) {
  int base = (matrix == 0) ? 0 : MATRIX_LEDS;
  for (int i = 0; i < MATRIX_LEDS; i++) {
    leds[base + i] = CRGB::Black;
  }
}

void MatrixDisplay::fadeAll(uint8_t amount) {
  for (int i = 0; i < TOTAL_LEDS; i++) {
    leds[i].nscale8(amount);
  }
}

int MatrixDisplay::xyToIndex(int x, int y) {
  // Row-major serpentine wiring
  // Physical LED 0 is at bottom-left
  // But we want coordinate (0,0) to be TOP-left for normal drawing
  // So invert Y-axis
  
  int physicalY = (ROWS - 1) - y;  // Flip Y-axis
  int index;
  
  if (physicalY % 2 == 0) {
    // Even rows: left to right
    index = physicalY * COLS + x;
  } else {
    // Odd rows: right to left (reversed)
    index = physicalY * COLS + (COLS - 1 - x);
  }
  
  return index;
}

int MatrixDisplay::getIndex(int matrix, int x, int y) {
  if (x < 0 || x >= COLS || y < 0 || y >= ROWS) {
    return -1;
  }
  
  int base = (matrix == 0) ? 0 : MATRIX_LEDS;
  int localIndex = xyToIndex(x, y);
  
  return base + localIndex;
}

void MatrixDisplay::setPixel(int matrix, int x, int y, CRGB color) {
  int idx = getIndex(matrix, x, y);
  if (idx >= 0 && idx < TOTAL_LEDS) {
    leds[idx] = color;
  }
}

CRGB MatrixDisplay::getPixel(int matrix, int x, int y) {
  int idx = getIndex(matrix, x, y);
  return (idx >= 0 && idx < TOTAL_LEDS) ? leds[idx] : CRGB::Black;
}

void MatrixDisplay::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
}

// V15.2.3-2026-01-04T15:00:00Z - Get matrix dimensions for auto-scaling/centering
int MatrixDisplay::getMatrixRows(int matrix) {
  if (matrix == 0) return MATRIX1_ROWS;  // V15.2.3-2026-01-04T15:00:00Z
  if (matrix == 1) return MATRIX2_ROWS;  // V15.2.3-2026-01-04T15:00:00Z
  #if ENABLE_MEGAMATRIX
    if (matrix == 2) return MATRIX3_ROWS;  // V15.2.3-2026-01-04T15:00:00Z
  #endif
  return MATRIX1_ROWS;  // V15.2.3-2026-01-04T15:00:00Z - Default fallback
}

int MatrixDisplay::getMatrixCols(int matrix) {
  if (matrix == 0) return MATRIX1_COLS;  // V15.2.3-2026-01-04T15:00:00Z
  if (matrix == 1) return MATRIX2_COLS;  // V15.2.3-2026-01-04T15:00:00Z
  #if ENABLE_MEGAMATRIX
    if (matrix == 2) return MATRIX3_COLS;  // V15.2.3-2026-01-04T15:00:00Z
  #endif
  return MATRIX1_COLS;  // V15.2.3-2026-01-04T15:00:00Z - Default fallback
}

// v2.1: Added circle drawing implementation using midpoint circle algorithm
void MatrixDisplay::drawCircle(int matrix, int cx, int cy, int radius, CRGB color, bool filled) {
  if (filled) {
    // Draw filled circle
    for (int y = -radius; y <= radius; y++) {
      for (int x = -radius; x <= radius; x++) {
        if (x*x + y*y <= radius*radius) {
          setPixel(matrix, cx + x, cy + y, color);
        }
      }
    }
  } else {
    // Draw circle outline using midpoint circle algorithm
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
      setPixel(matrix, cx + x, cy + y, color);
      setPixel(matrix, cx + y, cy + x, color);
      setPixel(matrix, cx - y, cy + x, color);
      setPixel(matrix, cx - x, cy + y, color);
      setPixel(matrix, cx - x, cy - y, color);
      setPixel(matrix, cx - y, cy - x, color);
      setPixel(matrix, cx + y, cy - x, color);
      setPixel(matrix, cx + x, cy - y, color);
      
      if (err <= 0) {
        y += 1;
        err += 2*y + 1;
      }
      
      if (err > 0) {
        x -= 1;
        err -= 2*x + 1;
      }
    }
  }
}