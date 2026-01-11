/* MatrixDisplay.h
   Low-level display management and coordinate mapping
   VERSION: V15.2.3-2026-01-04T15:00:00Z - Added matrix dimension getters for auto-scaling
   
   V15.2.3-2026-01-04T15:00:00Z - Added getMatrixRows/Cols for per-matrix sizing
   v2.1 - Added drawCircle method
*/

#ifndef MATRIX_DISPLAY_H
#define MATRIX_DISPLAY_H

#include "Config.h"

class MatrixDisplay {
public:
  MatrixDisplay();
  void begin();
  void show();
  void clear();
  void clearMatrix(int matrix);
  void fadeAll(uint8_t amount);
  
  // Direct LED access
  CRGB* getLeds() { return leds; }
  
  // Coordinate mapping
  int getIndex(int matrix, int x, int y);
  void setPixel(int matrix, int x, int y, CRGB color);
  CRGB getPixel(int matrix, int x, int y);
  
  // v2.1: Added circle drawing method
  void drawCircle(int matrix, int cx, int cy, int radius, CRGB color, bool filled = false);
  
  // V15.2.3-2026-01-04T15:00:00Z - Get matrix dimensions for auto-scaling
  int getMatrixRows(int matrix);
  int getMatrixCols(int matrix);
  
  // Utility
  void setBrightness(uint8_t brightness);
  
private:
  CRGB leds[TOTAL_LEDS];
  int xyToIndex(int x, int y);
};

#endif