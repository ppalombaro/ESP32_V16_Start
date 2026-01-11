@echo off
title JSON Data -> Custom Flash Storage
setlocal ENABLEDELAYEDEXPANSION

REM ============================
REM CONFIGURATION
REM V16.1.3-2026-01-09T05:00:00Z - Custom flash storage (final)
REM ============================
set PROJECT_DIR=D:\patpa\Documents\Arduino\ESP32_MatrixShow
set PROJECT_TOOLS=%PROJECT_DIR%\tools\FFAT
set DATA_IN=%PROJECT_DIR%\data_in
set DATA_OUT=%PROJECT_DIR%\data
set IMAGE_OUT=%PROJECT_DIR%\ffat.bin
set MANIFEST_OUT=%PROJECT_DIR%\ffat_manifest.txt

set PORT=COM3
set BAUD=921600
set FFAT_SIZE=917504

REM Path to esptool.exe
set ESPTOOL=C:\Users\patpa\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\4.5.1\esptool.exe

REM Python executable
set PYTHON=python

REM Check for dry-run flag
set DRYRUN=0
if "%1"=="--dry-run" set DRYRUN=1

echo ========================================
echo JSON to Custom Flash Storage
echo V16.1.3-2026-01-09T05:00:00Z - FINAL
echo ========================================
echo.

REM ============================
REM CLEAN DATA OUT
REM ============================
if exist "%DATA_OUT%" rmdir /s /q "%DATA_OUT%"
mkdir "%DATA_OUT%"

REM ============================
REM MINIFY JSON FILES
REM ============================
echo Step 1: Minifying JSON files...
%PYTHON% "%PROJECT_TOOLS%\minify_json.py" "%DATA_IN%" "%DATA_OUT%"
if errorlevel 1 (
    echo ERROR: Minification failed
    pause
    goto :EOF
)
echo Minification complete!
pause

REM ============================
REM GENERATE MANIFEST
REM ============================
echo.
echo Step 2: Generating manifest...
%PYTHON% "%PROJECT_TOOLS%\generate_manifest.py" "%DATA_OUT%" "%MANIFEST_OUT%"
if errorlevel 1 (
    echo ERROR: Manifest generation failed
    pause
    goto :EOF
)
echo Manifest complete!
pause

if %DRYRUN%==1 (
    echo Dry-run complete. No image created.
    pause
    goto :EOF
)

REM ============================
REM BUILD SIMPLE STORAGE IMAGE
REM V16.1.3-2026-01-09T05:10:00Z - Simple indexed format
REM ============================
echo.
echo Step 3: Building simple storage image...
%PYTHON% "%PROJECT_TOOLS%\build_simple_storage.py" "%DATA_OUT%" "%IMAGE_OUT%" %FFAT_SIZE% --manifest "%MANIFEST_OUT%"
if errorlevel 1 (
    echo ERROR: Image build failed
    pause
    goto :EOF
)

if not exist "%IMAGE_OUT%" (
    echo ERROR: Image file not created!
    pause
    goto :EOF
)

echo Image created successfully!
pause

REM ============================
REM FLASH IMAGE TO ESP32
REM ============================
echo.
echo Step 4: Flashing image to ESP32...
"%ESPTOOL%" --chip esp32 --port %PORT% --baud %BAUD% write_flash 0x290000 "%IMAGE_OUT%"
if errorlevel 1 (
    echo ERROR: Flash failed
    pause
    goto :EOF
)
echo Flash complete!
pause

REM ============================
REM CLEAN STAGING FOLDER
REM ============================
echo.
echo Cleaning staging folder...
if exist "%DATA_OUT%" rmdir /s /q "%DATA_OUT%"

echo.
echo ========================================
echo SUCCESS! Flash storage updated!
echo ========================================
echo.
echo Next steps:
echo 1. Upload ESP32_MatrixShow sketch (with updated ContentManager)
echo 2. Open Serial Monitor (115200 baud)
echo 3. Press ESP32 reset button
echo 4. Look for: Files in storage: 120
echo              Discovered 5 themes
echo.
pause
endlocal
