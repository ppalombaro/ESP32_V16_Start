#include "SceneData.h"
#include <FS.h>
#include <FFat.h>
#include <memory> // V16.1.1-2026-01-08T14:40:00Z

SceneData::SceneData(const String& filePath)
    : _filePath(filePath) {}

bool SceneData::load() {
    if (_filePath.length() == 0) {
        Logger::instance().log("[SceneData] Empty file path");
        return false;
    }

    // V16.1.1-2026-01-08T14:40:00Z - Ensure FFat is mounted
    if (!FFat.begin(true)) {
        Logger::instance().log("[SceneData] FFat mount failed");
        return false;
    }

    File file = FFat.open(_filePath, "r");
    if (!file) {
        Logger::instance().log("[SceneData] File not found: " + _filePath);
        return false;
    }

    const size_t bufferSize = file.size() + 1;
    std::unique_ptr<char[]> buf(new char[bufferSize]);
    file.readBytes(buf.get(), bufferSize);
    file.close();

    DynamicJsonDocument doc(4096);
    auto err = deserializeJson(doc, buf.get());
    if (err) {
        Logger::instance().log("[SceneData] JSON error: " + String(err.c_str()));
        return false;
    }

    if (!doc.containsKey("frames")) {
        Logger::instance().log("[SceneData] Missing frames in: " + _filePath);
        return false;
    }

    _frames.clear();
    for (auto&& frame : doc["frames"].as<JsonArray>()) {
        FrameData f;
        f.durationMs = frame["duration"];
        f.content = frame["content"].as<String>();
        _frames.push_back(f);
    }

    Logger::instance().log("[SceneData] Loaded " + String(_frames.size()) + " frames");
    return true;
}
