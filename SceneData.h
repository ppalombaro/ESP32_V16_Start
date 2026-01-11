#pragma once
#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>
#include "Logger.h"

struct FrameData {
    uint16_t durationMs;
    String content;
};

class SceneData {
public:
    SceneData(const String& filePath);

    bool load();  // Loads JSON from FFAT or SPIFFS
    const std::vector<FrameData>& getFrames() const { return _frames; }

private:
    String _filePath;
    std::vector<FrameData> _frames;
};
