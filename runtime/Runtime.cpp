#include "runtime/Runtime.h"
#include "utils/Profiler.h"

#include "mINI/ini.h"

#include <Windows.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <unordered_map>
#include <string>

// Predefined minimap regions and pixel-to-meter scales for supported resolutions
struct ResolutionProfiles {
    MiniMapRegion minimapRegion;
    double pixelPer100m;
    Point defaultOrigin;
    int fullScreenRight;
    int fullScreenBottom;
};

// Resolution profiles for supported resolutions
// Format: "WIDTHxHEIGHT": {minimapRegion, pixelPer100m, MiniMapDefaultOrigin, FullScreenRight, FullScreenBottom}
static const std::unordered_map<std::string, ResolutionProfiles> g_resolutionProfiles = {
    {"1920x1080", {{1427, 590, 1888, 1052}, 65.8, {1650, 850}, 1920, 1080}},
    {"2560x1440", {{1905, 790, 2515, 1400}, 87.5, {2210, 1095}, 2560, 1440}},
};

// Predefined color profiles for different types of color blindness
static const std::unordered_map<std::string, std::array<ColorRGB, 4>> g_colorProfiles = {
    {"none", {
        ColorRGB{233, 229, 17},
        ColorRGB{218, 98, 38},
        ColorRGB{58, 160, 217},
        ColorRGB{68, 181, 73}
    }},
    {"default", {
        ColorRGB{233, 229, 17},
        ColorRGB{218, 98, 38},
        ColorRGB{58, 160, 217},
        ColorRGB{68, 181, 73}
    }},
    {"deuteranopia", {
        ColorRGB{240, 228, 66},
        ColorRGB{213, 94, 0},
        ColorRGB{86, 180, 233},
        ColorRGB{0, 158, 115}
    }},
    {"protanopia", {
        ColorRGB{230, 159, 0},
        ColorRGB{204, 121, 167},
        ColorRGB{86, 180, 233},
        ColorRGB{0, 114, 178}
    }},
    {"tritanopia", {
        ColorRGB{213, 94, 0},
        ColorRGB{204, 121, 167},
        ColorRGB{0, 158, 115},
        ColorRGB{0, 114, 178}
    }},
};

static std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

static std::string detectCurrentResolution() {
    const int width = GetSystemMetrics(SM_CXSCREEN);
    const int height = GetSystemMetrics(SM_CYSCREEN);
    return std::to_string(width) + "x" + std::to_string(height);
}

bool Runtime::initialize() {
    std::cout << "[Runtime] Loading config...\n";

    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;

    if(!file.read(ini)) {
        std::cerr << "[Runtime] Failed to read config.ini" << std::endl;
        return false;
    }
    
    config.resolution = detectCurrentResolution();
    config.measureOverlayHotKey = ini["CONFIG"]["measureOverlayHotKey"];
    config.minimapOverlayHotkey = ini["CONFIG"]["minimapOverlayHotkey"];
    config.colorBlind = toLower(ini["CONFIG"]["colorBlind"]);
    if (config.colorBlind.empty()) {
        config.colorBlind = "none";
    }

    const std::string debugValue = toLower(ini["CONFIG"]["debug"]);
    const bool debugEnabled = (debugValue == "true" || debugValue == "1" || debugValue == "yes" || debugValue == "on");
    Profiler::instance().setEnabled(debugEnabled);

    config.tolerance = std::stoi(ini["CONFIG"]["tolerance"]);

    const auto colorIt = g_colorProfiles.find(config.colorBlind);
    if (colorIt != g_colorProfiles.end()) {
        config.markerColors = colorIt->second;
    } else {
        std::cerr << "[Runtime] Unknown colorBlind mode, fallback to none\n";
        config.markerColors = g_colorProfiles.at("none");
        config.colorBlind = "none";
    }

    auto it = g_resolutionProfiles.find(config.resolution);

    if (it != g_resolutionProfiles.end())
    {
        std::cout << "[Runtime] Current resolution: " << config.resolution << "\n";
        config.minimapRegion = it->second.minimapRegion;
        config.pixelPer100m = it->second.pixelPer100m;
        config.defaultOrigin = it->second.defaultOrigin;
        config.fullScreenRight = it->second.fullScreenRight;
        config.fullScreenBottom = it->second.fullScreenBottom;
    }
    else
    {
        const std::string detectedResolution = config.resolution;
        std::cerr << "[Runtime] Unsupported resolution " << detectedResolution
                  << ", fallback minimap profile to 1920x1080\n";

        config.minimapRegion = g_resolutionProfiles.at("1920x1080").minimapRegion;
        config.pixelPer100m = g_resolutionProfiles.at("1920x1080").pixelPer100m;
        config.defaultOrigin = g_resolutionProfiles.at("1920x1080").defaultOrigin;
        config.fullScreenRight = g_resolutionProfiles.at("1920x1080").fullScreenRight;
        config.fullScreenBottom = g_resolutionProfiles.at("1920x1080").fullScreenBottom;
    }

    std::cout << "[Runtime] Runtime initialized\n";
    return true;
}
