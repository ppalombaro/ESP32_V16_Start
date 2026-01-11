/* WebController.cpp
   Web server implementation
   VERSION: V16.2.5-2026-01-10T22:18:00Z - Created missing implementation
*/

#include "WebController.h"
#include "WebPages.h"
#include "WebActions.h"
#include "ContentManager.h"
#include "ThemeManager.h"
#include "MatrixDisplay.h"

WebController::WebController() : server(80) {}

void WebController::begin(ContentManager* contentMgr, ThemeManager* themeMgr, MatrixDisplay* disp) {
    content = contentMgr;
    themes = themeMgr;
    display = disp;
    
    setupRoutes();
    
    // V16.2.5-2026-01-10T22:18:00Z - Create WebActions
    actions = new WebActions(content, themes, display, &server);
    actions->attach();
    
    server.begin();
    Serial.println("[WebController] Server started on port 80");
}

void WebController::handle() {
    server.handleClient();
}

void WebController::setupRoutes() {
    // V16.2.5-2026-01-10T22:18:00Z - Setup page routes
    server.on("/", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildRootPage());
    });
    
    server.on("/control", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildControlPage(content));
    });
    
    server.on("/schedule", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildSchedulePage(content));
    });
    
    server.on("/times", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildTimesPage());
    });
    
    server.on("/logs", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildLogsPage());
    });
    
    server.on("/discovery", HTTP_GET, [this]() {
        server.send(200, "text/html", WebPages::buildDiscoveryPage(content));
    });
}