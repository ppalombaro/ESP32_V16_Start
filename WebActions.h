/* WebActions.h
   API endpoints for web interface
   VERSION: V16.1.3-2026-01-09T05:25:00Z - Added brightness control
*/

#pragma once

#include <WebServer.h>

class ContentManager;
class ThemeManager;
class MatrixDisplay;

class WebActions {
public:
    WebActions(ContentManager* cm, ThemeManager* tm, MatrixDisplay* disp, WebServer* srv);
    void attach();

private:
    ContentManager* contentMgr;
    ThemeManager* themeMgr;
    MatrixDisplay* display;
    WebServer* server;
    
    void saveBrightness(uint8_t brightness);
    uint8_t loadBrightness();
};
