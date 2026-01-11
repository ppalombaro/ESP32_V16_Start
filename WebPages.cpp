/* WebPages.cpp
   HTML page generation for web interface
   VERSION: V16.1.3-2026-01-09T05:20:00Z - Complete UI rebuild
*/

#include "WebPages.h"
#include "ContentManager.h"
#include "Logger.h"

// Common HTML header with styling
String WebPages::htmlHeader(const String& title) {
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='utf-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>" + title + "</title>";
    html += "<style>";
    html += "body{font-family:Arial,sans-serif;background:#1a1a1a;color:#eee;margin:0;padding:20px;}";
    html += ".container{max-width:1200px;margin:0 auto;}";
    html += "h1{color:#4CAF50;margin-top:0;}";
    html += "h2{color:#81C784;border-bottom:2px solid #4CAF50;padding-bottom:10px;}";
    html += ".nav{background:#2d2d2d;padding:15px;margin-bottom:20px;border-radius:5px;}";
    html += ".nav a{color:#4CAF50;text-decoration:none;margin:0 15px;font-weight:bold;}";
    html += ".nav a:hover{color:#81C784;}";
    html += ".section{background:#2d2d2d;padding:20px;margin-bottom:20px;border-radius:5px;}";
    html += ".content-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(250px,1fr));gap:15px;}";
    html += ".content-item{background:#3d3d3d;padding:15px;border-radius:5px;border-left:4px solid #4CAF50;}";
    html += ".content-item h3{margin:0 0 10px 0;color:#81C784;}";
    html += ".content-item button{width:100%;padding:10px;margin-top:10px;background:#4CAF50;color:#fff;border:none;border-radius:3px;cursor:pointer;font-size:14px;}";
    html += ".content-item button:hover{background:#66BB6A;}";
    html += ".checkbox-item{background:#3d3d3d;padding:12px;margin:8px 0;border-radius:5px;display:flex;align-items:center;}";
    html += ".checkbox-item input{margin-right:12px;width:20px;height:20px;}";
    html += ".checkbox-item label{cursor:pointer;flex-grow:1;}";
    html += ".control-btn{padding:12px 24px;margin:8px;background:#4CAF50;color:#fff;border:none;border-radius:5px;cursor:pointer;font-size:16px;}";
    html += ".control-btn:hover{background:#66BB6A;}";
    html += ".danger-btn{background:#f44336;}";
    html += ".danger-btn:hover{background:#e53935;}";
    html += ".status{padding:15px;background:#3d3d3d;border-radius:5px;margin:15px 0;}";
    html += ".log-entry{font-family:monospace;background:#3d3d3d;padding:8px;margin:5px 0;border-radius:3px;font-size:13px;}";
    html += "input[type=time],input[type=number]{padding:10px;background:#3d3d3d;color:#eee;border:1px solid #4CAF50;border-radius:3px;font-size:14px;}";
    html += "select{padding:10px;background:#3d3d3d;color:#eee;border:1px solid #4CAF50;border-radius:3px;font-size:14px;}";
    html += ".form-group{margin:15px 0;}";
    html += ".form-group label{display:block;margin-bottom:8px;color:#81C784;font-weight:bold;}";
    html += "</style>";
    html += "</head><body><div class='container'>";
    return html;
}

String WebPages::htmlFooter() {
    return "</div></body></html>";
}

String WebPages::buildNavigation() {
    String nav = "<div class='nav'>";
    nav += "<a href='/'>Control</a>";
    nav += "<a href='/schedule'>Schedule</a>";
    nav += "<a href='/times'>Schedule Times</a>";
    nav += "<a href='/logs'>Logs</a>";
    nav += "<a href='/discovery'>Discovery</a>";
    nav += "</div>";
    return nav;
}

// Page 1: Control/Preview Page
String WebPages::buildControlPage(ContentManager* content) {
    String html = htmlHeader("Matrix Control");
    html += buildNavigation();
    
    html += "<h1>Matrix Control</h1>";
    
    // Display controls
    html += "<div class='section'>";
    html += "<h2>Display Controls</h2>";
    html += "<div style='display:inline-block;margin:10px;'>";
    html += "<label style='display:block;color:#81C784;margin-bottom:8px;'>Brightness: <span id='brightnessValue'>--</span></label>";
    html += "<input type='range' id='brightness' min='1' max='255' value='20' style='width:200px;' oninput='updateBrightness(this.value)'>";
    html += "</div>";
    html += "<br>";
    html += "<button class='control-btn danger-btn' onclick='clearDisplay()'>?????EUR? Clear Display</button>";
    html += "<button class='control-btn' onclick='testPattern()'>???,?'?+ Test Pattern</button>";
    html += "</div>";
    
    // Get all content grouped by theme
    auto themes = content->getDiscoveredThemes();
    
    for (const auto& theme : themes) {
        auto items = content->getContentByTheme(theme);
        if (items.size() == 0) continue;
        
        html += "<div class='section'>";
        html += "<h2>Theme: " + theme + "</h2>";
        html += "<div class='content-grid'>";
        
        for (const auto& item : items) {
            // Skip test patterns on main page
            if (item.type == CONTENT_TEST) continue;
            
            html += "<div class='content-item'>";
            html += "<h3>" + item.name + "</h3>";
            html += "<p style='margin:5px 0;color:#aaa;font-size:13px;'>";
            
            switch(item.type) {
                case CONTENT_SCENE: html += "???,?EURoe?? Scene"; break;
                case CONTENT_ANIMATION: html += "???,? 1/2 ?? Animation"; break;
                case CONTENT_SCROLL: html += "???,?EURoe?" Scroll"; break;
                case CONTENT_PROCEDURAL: html += "?????,,????,?? Procedural"; break;
            }
            
            html += "</p>";
            html += "<button onclick='preview(" + String(item.id) + ")'>???EUR"?????,?? Preview</button>";
            html += "</div>";
        }
        
        html += "</div></div>";
    }
    
    // JavaScript - V16.2.1-2026-01-10T18:52:00Z - Removed annoying alert() popups
    html += "<script>";
    html += "function preview(id){fetch('/api/render?id='+id);}";
    html += "function clearDisplay(){fetch('/api/clear');}";
    html += "function testPattern(){fetch('/api/test');}";
    html += "function updateBrightness(val){";
    html += "  document.getElementById('brightnessValue').innerText=val;";
    html += "  fetch('/api/brightness?value='+val);";
    html += "}";
    html += "window.onload=function(){";
    html += "  fetch('/api/brightness/get').then(r=>r.text()).then(val=>{";
    html += "    document.getElementById('brightness').value=val;";
    html += "    document.getElementById('brightnessValue').innerText=val;";
    html += "  });";
    html += "}";
    html += "</script>";
    
    html += htmlFooter();
    return html;
}

// Page 2: Schedule Configuration
String WebPages::buildSchedulePage(ContentManager* content) {
    String html = htmlHeader("Schedule Config");
    html += buildNavigation();
    
    html += "<h1>???,?EURoe?EUR? Random Schedule Configuration</h1>";
    
    // Random mode status
    html += "<div class='section'>";
    html += "<h2>Random Mode Status</h2>";
    html += "<div class='status'>";
    html += "<p><strong>Status:</strong> " + String(content->isRandomModeEnabled() ? "???"?EUR? ENABLED" : "???EUR??EUR? DISABLED") + "</p>";
    html += "<p><strong>Interval:</strong> " + String(content->getRandomInterval()) + " ms</p>";
    html += "<p><strong>Theme Filter:</strong> " + (content->getRandomThemeFilter().length() > 0 ? content->getRandomThemeFilter() : "ALL THEMES") + "</p>";
    html += "</div>";
    
    if (content->isRandomModeEnabled()) {
        html += "<button class='control-btn danger-btn' onclick='toggleRandom(false)'>???EUR??EUR? Disable Random Mode</button>";
    } else {
        html += "<button class='control-btn' onclick='toggleRandom(true)'>???"?EUR? Enable Random Mode</button>";
    }
    html += "</div>";
    
    // Random settings
    html += "<div class='section'>";
    html += "<h2>Random Settings</h2>";
    html += "<div class='form-group'>";
    html += "<label>Change Interval (milliseconds):</label>";
    html += "<input type='number' id='interval' value='" + String(content->getRandomInterval()) + "' min='1000' step='1000'>";
    html += "<button class='control-btn' onclick='setInterval()' style='margin-left:10px;'>Update</button>";
    html += "</div>";
    
    html += "<div class='form-group'>";
    html += "<label>Theme Filter:</label>";
    html += "<select id='themeFilter' onchange='setFilter()'>";
    html += "<option value=''>ALL THEMES</option>";
    
    auto themes = content->getDiscoveredThemes();
    String currentFilter = content->getRandomThemeFilter();
    for (const auto& theme : themes) {
        html += "<option value='" + theme + "'";
        if (theme == currentFilter) html += " selected";
        html += ">" + theme + "</option>";
    }
    html += "</select>";
    html += "</div>";
    html += "</div>";
    
    // Eligible content selection
    html += "<div class='section'>";
    html += "<h2>Eligible Content</h2>";
    html += "<p style='color:#aaa;'>Select which content can be included in random schedule:</p>";
    
    auto allContent = content->getContent();
    for (const auto& item : allContent) {
        // Skip test patterns
        if (item.type == CONTENT_TEST) continue;
        
        html += "<div class='checkbox-item'>";
        html += "<input type='checkbox' id='content_" + String(item.id) + "' checked>";
        html += "<label for='content_" + String(item.id) + "'>";
        html += "<strong>" + item.name + "</strong> [" + item.theme + "] ";
        
        switch(item.type) {
            case CONTENT_SCENE: html += "(scene)"; break;
            case CONTENT_ANIMATION: html += "(animation)"; break;
            case CONTENT_SCROLL: html += "(scroll)"; break;
            case CONTENT_PROCEDURAL: html += "(procedural)"; break;
        }
        
        html += "</label>";
        html += "</div>";
    }
    
    html += "<button class='control-btn' onclick='saveEligible()' style='margin-top:20px;'>???,?EUR(TM)? 3/4  Save Eligible Content</button>";
    html += "</div>";
    
    // JavaScript
    html += "<script>";
    html += "function toggleRandom(enable){";
    html += "  fetch('/api/random/'+(enable?'enable':'disable')).then(()=>location.reload());";
    html += "}";
    html += "function setInterval(){";
    html += "  let ms=document.getElementById('interval').value;";
    html += "  fetch('/api/random/interval?ms='+ms);";
    html += "}";
    html += "function setFilter(){";
    html += "  let theme=document.getElementById('themeFilter').value;";
    html += "  fetch('/api/random/filter?theme='+theme);";
    html += "}";
    html += "function saveEligible(){";
    html += "  console.log('Eligible content saved (TODO: implement persistence)');";
    html += "}";
    html += "</script>";
    
    html += htmlFooter();
    return html;
}

// Page 3: Schedule Times
String WebPages::buildTimesPage() {
    String html = htmlHeader("Schedule Times");
    html += buildNavigation();
    
    html += "<h1>?????? Schedule Times</h1>";
    
    html += "<div class='section'>";
    html += "<h2>Daily Schedule</h2>";
    html += "<p style='color:#aaa;'>Configure when the random schedule should run each day:</p>";
    
    html += "<div class='form-group'>";
    html += "<label>Start Time:</label>";
    html += "<input type='time' id='startTime' value='17:00'>";
    html += "</div>";
    
    html += "<div class='form-group'>";
    html += "<label>End Time:</label>";
    html += "<input type='time' id='endTime' value='22:00'>";
    html += "</div>";
    
    html += "<button class='control-btn' onclick='saveTimes()'>???,?EUR(TM)? 3/4  Save Schedule Times</button>";
    html += "</div>";
    
    html += "<div class='section'>";
    html += "<h2>Current Schedule</h2>";
    html += "<div class='status'>";
    html += "<p><strong>Active Hours:</strong> 17:00 - 22:00</p>";
    html += "<p><strong>Status:</strong> Schedule times not yet implemented</p>";
    html += "</div>";
    html += "</div>";
    
    // JavaScript
    html += "<script>";
    html += "function saveTimes(){";
    html += "  let start=document.getElementById('startTime').value;";
    html += "  let end=document.getElementById('endTime').value;";
    html += "  console.log('Schedule times: '+start+' to '+end+' (TODO: implement persistence)');";
    html += "}";
    html += "</script>";
    
    html += htmlFooter();
    return html;
}

// Page 4: Logs
String WebPages::buildLogsPage() {
    String html = htmlHeader("System Logs");
    html += buildNavigation();
    
    html += "<h1>???,?EURoe?EUR? System Logs</h1>";
    
    html += "<div class='section'>";
    html += "<h2>Recent Log Entries</h2>";
    html += "<button class='control-btn' onclick='location.reload()'>???,?EUR??EUR? Refresh</button>";
    html += "<button class='control-btn danger-btn' onclick='clearLogs()'>???,?EUR"?EUR~???,?? Clear Logs</button>";
    
    html += "<div style='margin-top:20px;'>";
    
    auto logs = Logger::instance().getRecentLogs();
    if (logs.size() == 0) {
        html += "<p style='color:#aaa;'>No log entries</p>";
    } else {
        for (const auto& log : logs) {
            html += "<div class='log-entry'>" + log + "</div>";
        }
    }
    
    html += "</div></div>";
    
    // JavaScript
    html += "<script>";
    html += "function clearLogs(){";
    html += "  if(confirm('Clear all logs?')){";
    html += "    fetch('/api/logs/clear').then(()=>location.reload());";
    html += "  }";
    html += "}";
    html += "</script>";
    
    html += htmlFooter();
    return html;
}

// Discovery page (existing)
String WebPages::buildDiscoveryPage(ContentManager* content) {
    String html = htmlHeader("Content Discovery");
    html += buildNavigation();
    
    html += "<h1>???,?EUR??? Content Discovery</h1>";
    
    html += "<div class='section'>";
    html += "<h2>Discovered Themes</h2>";
    auto themes = content->getDiscoveredThemes();
    html += "<p><strong>Total:</strong> " + String(themes.size()) + "</p>";
    for (const auto& theme : themes) {
        html += "<div class='status'>" + theme + "</div>";
    }
    html += "</div>";
    
    html += "<div class='section'>";
    html += "<h2>All Content</h2>";
    auto allContent = content->getContent();
    html += "<p><strong>Total Items:</strong> " + String(allContent.size()) + "</p>";
    
    html += "<div class='content-grid'>";
    for (const auto& item : allContent) {
        html += "<div class='content-item'>";
        html += "<h3>" + item.name + "</h3>";
        html += "<p style='margin:5px 0;color:#aaa;'>Theme: " + item.theme + "</p>";
        html += "<p style='margin:5px 0;color:#aaa;'>ID: " + String(item.id) + "</p>";
        html += "</div>";
    }
    html += "</div></div>";
    
    html += htmlFooter();
    return html;
}

String WebPages::buildRootPage() {
    // Redirect to control page
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta http-equiv='refresh' content='0;url=/control'>";
    html += "</head><body>Redirecting...</body></html>";
    return html;
}