ESP32 Matrix Show - Custom Flash Storage Tools
V16.1.3-2026-01-09T05:00:00Z
===============================================

WHAT'S IN THIS FOLDER:
======================
1. json_datain_2_FFAT.bat          - Main upload script
2. minify_json.py                   - Minifies JSON files
3. build_ffat_custom.py             - Creates custom flash image
4. generate_manifest.py             - Creates verification manifest

INSTALLATION:
=============
1. Extract all files to:
   D:\patpa\Documents\Arduino\ESP32_MatrixShow\tools\FFAT\

2. Make sure you have the updated ContentManager files:
   - ContentManager.cpp (V16.1.3-2026-01-09T05:00:00Z)
   - ContentManager.h (V16.1.3-2026-01-09T05:00:00Z)

USAGE:
======
1. Run: tools\FFAT\json_datain_2_FFAT.bat

2. The script will:
   - Minify your JSON files from data_in/
   - Build a custom flash image
   - Flash it to ESP32 at 0x290000
   - Generate verification manifest

3. Upload your sketch (with updated ContentManager)

4. Check Serial Monitor for:
   [ContentManager] Files in storage: 120
   [ContentManager] Discovered 5 themes

WHAT THIS DOES:
===============
- Stores JSON files in custom format on ESP32 flash
- No FFat/SPIFFS filesystem needed
- Direct flash reading using esp_flash_read()
- Fast discovery and access
- Update anytime by re-running the BAT

REQUIREMENTS:
=============
- Python 3.x
- esptool (already installed with Arduino ESP32)
- Your data_in/ folder with JSON files

NO EXTERNAL LIBRARIES NEEDED!
