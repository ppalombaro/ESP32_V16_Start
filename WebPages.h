/* WebPages.h
   HTML page generation for web interface
   VERSION: V16.1.3-2026-01-09T05:20:00Z - Complete UI rebuild
*/

#pragma once

#include <Arduino.h>

class ContentManager;

class WebPages {
public:
    static String buildRootPage();
    static String buildControlPage(ContentManager* content);
    static String buildSchedulePage(ContentManager* content);
    static String buildTimesPage();
    static String buildLogsPage();
    static String buildDiscoveryPage(ContentManager* content);

private:
    static String htmlHeader(const String& title);
    static String htmlFooter();
    static String buildNavigation();
};
