/* Config.h
   Hardware configuration and global settings
   VERSION: V16.1.2-2026-01-08T15:00:00Z - Content auto-discovery architecture
   
   Matrix 0 = Right Window Matrix - PIN 16 (ACTIVE)
   Matrix 1 = Left Window Matrix - PIN 17 (ACTIVE)
   Matrix 2 = Mega Matrix - PIN 18 (DISABLED - future)
   Mega Tree = PIN 19 (DISABLED - future)
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <FastLED.h>
 
// ============WiFi Configuration - EDIT THESE
#define WIFI_SSID "palombaro"
#define WIFI_PASSWORD "boobug00"

#define HOSTNAME "palombaro-matrix"

// ========== FEATURE ENABLE/DISABLE FLAGS ==========
#define ENABLE_MEGAMATRIX false  // V16.1.2 - Matrix 2 disabled (future)
#define ENABLE_MEGATREE false    // V16.1.2 - Mega tree disabled (future)

// ========== HARDWARE CONFIGURATION ==========

// Matrix 0 Configuration (Right Window - ACTIVE)
#define MATRIX0_ROWS 25
#define MATRIX0_COLS 20
#define MATRIX0_LEDS (MATRIX0_ROWS * MATRIX0_COLS)  // 500
#define PIN_MATRIX0 16

// Matrix 1 Configuration (Left Window - ACTIVE)
#define MATRIX1_ROWS 25
#define MATRIX1_COLS 20
#define MATRIX1_LEDS (MATRIX1_ROWS * MATRIX1_COLS)  // 500
#define PIN_MATRIX1 17

// V16.1.2 - Matrix 2 Configuration (Mega Matrix - DISABLED)
#define MATRIX2_ROWS 40
#define MATRIX2_COLS 50
#define MATRIX2_LEDS (MATRIX2_ROWS * MATRIX2_COLS)  // 2000
#define PIN_MATRIX2 18

// V16.1.2 - Mega Tree Configuration (DISABLED)
#define PIN_MEGATREE 19
#define MEGATREE_LEDS 1000       // 20 branches Ã— 50 LEDs
#define MEGATREE_BRANCHES 20
#define MEGATREE_LEDS_PER_BRANCH 50

// Legacy compatibility names
#define MATRIX1_ROWS MATRIX0_ROWS
#define MATRIX1_COLS MATRIX0_COLS
#define MATRIX1_LEDS MATRIX0_LEDS
#define MATRIX2_ROWS MATRIX1_ROWS
#define MATRIX2_COLS MATRIX1_COLS
#define MATRIX2_LEDS MATRIX1_LEDS
#define ROWS MATRIX0_ROWS
#define COLS MATRIX0_COLS
#define MATRIX_LEDS MATRIX0_LEDS
#define PIN_LEFT PIN_MATRIX1
#define PIN_RIGHT PIN_MATRIX0

// V16.1.2 - Total LED count calculation
#if ENABLE_MEGAMATRIX
  #define TOTAL_LEDS (MATRIX0_LEDS + MATRIX1_LEDS + MATRIX2_LEDS)
#else
  #define TOTAL_LEDS (MATRIX0_LEDS + MATRIX1_LEDS)  // 1000 (current setup)
#endif

#define COLOR_ORDER RGB
#define LED_TYPE WS2811

// Display Settings
#define DEFAULT_BRIGHTNESS 20

// V16.1.2 - Display intervals
#define STATIC_SCENE_INTERVAL 5000    // 5 seconds for static scenes
#define ANIMATION_INTERVAL 8000       // 8 seconds for animations
#define SCROLL_INTERVAL 10000         // 10 seconds for scroll text
#define SCENE_INTERVAL 5000           // Legacy compatibility

// --- Run Mode Definitions ---
#define RUN_MODE_MANUAL 0       
#define RUN_MODE_SCHEDULE 1     

// --- Theme Definitions ---
#define THEME_OFF 0
#define THEME_CHRISTMAS 1
#define THEME_HALLOWEEN 2
#define THEME_THANKSGIVING 3
#define THEME_NEWYEAR 4
#define THEME_OSU 5
#define THEME_RANDOM_MAGIC 10
#define THEME_TEST_MODE 99

// --- Content Type Definitions (V16.1.2) ---
#define CONTENT_TYPE_SCENE 1
#define CONTENT_TYPE_ANIMATION 2
#define CONTENT_TYPE_SCROLL 3
#define CONTENT_TYPE_PROCEDURAL 4
#define CONTENT_TYPE_TEST 5

// --- Preference Keys for Persistent Storage ---
#define PREFS_NAMESPACE "show-config"
#define RUN_MODE_KEY "run_mode"
#define START_HOUR_KEY "start_hr"
#define END_HOUR_KEY "end_hr"
#define BRIGHTNESS_KEY "brightness"
#define RANDOM_SCENES_MASK_KEY "rand_scns"
#define RANDOM_ANIMS_MASK_KEY "rand_anims"
#define RANDOM_TESTS_MASK_KEY "rand_tests"

// ========== THEME COLOR DEFINITIONS ==========

// Christmas Colors
#define CHRISTMAS_COLOR_1 CRGB(255, 0, 0)    // Red
#define CHRISTMAS_COLOR_2 CRGB(0, 255, 0)    // Green
#define CHRISTMAS_COLOR_3 CRGB(255, 255, 255) // White

// Halloween Colors
#define HALLOWEEN_COLOR_1 CRGB(255, 128, 0)  // Orange
#define HALLOWEEN_COLOR_2 CRGB(128, 0, 128)  // Purple
#define HALLOWEEN_COLOR_3 CRGB(0, 255, 0)    // Lime Green

// Thanksgiving Colors
#define THANKSGIVING_COLOR_1 CRGB(255, 140, 0)   // Dark Orange
#define THANKSGIVING_COLOR_2 CRGB(218, 165, 32)  // Goldenrod
#define THANKSGIVING_COLOR_3 CRGB(139, 69, 19)   // Brown

// New Year Colors
#define NEWYEAR_COLOR_1 CRGB(255, 215, 0)    // Gold
#define NEWYEAR_COLOR_2 CRGB(192, 192, 192)  // Silver
#define NEWYEAR_COLOR_3 CRGB(255, 255, 255)  // White

// Ohio State Colors
#define OSU_COLOR_1 CRGB(187, 0, 0)      // Scarlet Red
#define OSU_COLOR_2 CRGB(102, 102, 102)  // Gray
#define OSU_COLOR_3 CRGB(255, 255, 255)  // White

// --- Custom Colors (defined in Config.cpp) ---
extern const CRGB Peach;
extern const CRGB Sienna;
extern const CRGB RudolfBrown;
extern const CRGB RudolfTan;
extern const CRGB SnowWhite;
extern const CRGB CoolBlue;

#endif
