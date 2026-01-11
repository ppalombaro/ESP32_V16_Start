/* WebController.h
   Web server and HTTP interface
   VERSION: V16.1.3-2026-01-09T05:35:00Z - Fixed WebActions lifecycle
*/

#pragma once

#include <Arduino.h>
#include <WebServer.h>

class ContentManager;
class ThemeManager;
class MatrixDisplay;
class WebActions;

class WebController {
public:
    WebController();

    void begin(ContentManager* contentMgr, ThemeManager* themeMgr, MatrixDisplay* disp);
    void handle();

private:
    WebServer server{80};
    ContentManager* content = nullptr;
    ThemeManager* themes = nullptr;
    MatrixDisplay* display = nullptr;
    WebActions* actions = nullptr;  // V16.1.3-2026-01-09T05:35:00Z

    void setupRoutes();
};