/* Logger.h
   Logging system with circular buffer storage
   VERSION: V16.1.3-2026-01-09T05:20:00Z
*/

#pragma once
#include <Arduino.h>
#include <vector>

class Logger {
public:
    static Logger& instance() {
        static Logger _instance;
        return _instance;
    }

    void log(const String& message) {
        Serial.println(message);
        
        // Store in circular buffer
        if (logBuffer.size() >= maxLogs) {
            logBuffer.erase(logBuffer.begin());
        }
        logBuffer.push_back(message);
    }

    template<typename... Args>
    void logf(const char* fmt, Args... args) {
        char buf[256];
        snprintf(buf, sizeof(buf), fmt, args...);
        log(String(buf));
    }
    
    std::vector<String> getRecentLogs() const {
        return logBuffer;
    }
    
    void clear() {
        logBuffer.clear();
        log("[Logger] Log buffer cleared");
    }

private:
    Logger() { Serial.begin(115200); }
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::vector<String> logBuffer;
    const size_t maxLogs = 100;  // Keep last 100 log entries
};
