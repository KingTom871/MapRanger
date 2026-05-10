#include "runtime/Runtime.h"

#include "mINI/ini.h"

#include <iostream>
#include <unordered_map>
#include <string>

// Predefined minimap regions and pixel-to-meter scales for supported resolutions
struct ResolutionProfiles {
    MiniMapRegion minimapRegion;
    double pixelToMeterScale;
};

// Resolution profiles for supported resolutions
static const std::unordered_map<std::string, ResolutionProfiles> g_resolutionProfiles = {
    {"1920x1080", {{1427, 590, 1888, 1052}, 65.8}},
    {"2560x1440", {{1905, 780, 2515, 1400}, 87.5}},
};

bool Runtime::initialize() {
    std::cout << "[Runtime] Loading config...\n";

    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;

    if(!file.read(ini)) {
        std::cerr << "[Runtime] Failed to read config.ini" << std::endl;
        return false;
    }
    
    // Load configuration values
    config.resolution = ini["CONFIG"]["resolution"];
    config.measureOverlayHotKey = ini["CONFIG"]["measureOverlayHotKey"];
    config.minimapOverlayHotkey = ini["CONFIG"]["minimapOverlayHotkey"];
    config.colorBlind = ini["CONFIG"]["colorBlind"];
    config.tolerance = std::stoi(ini["CONFIG"]["tolerance"]);

    // Set minimap region and pixel-to-meter scale based on resolution
    auto it = g_resolutionProfiles.find(config.resolution);

    if (it != g_resolutionProfiles.end())
    {
        config.minimapRegion = it->second.minimapRegion;
        config.pixelToMeterScale = it->second.pixelToMeterScale;
    }
    else
    {
        std::cerr << "[Runtime] Unknown resolution, fallback to 1920x1080\n";

        config.minimapRegion = g_resolutionProfiles.at("1920x1080").minimapRegion;
        config.pixelToMeterScale = g_resolutionProfiles.at("1920x1080").pixelToMeterScale;
        config.resolution = "1920x1080";
    }

    std::cout << "[Runtime] Runtime initialized\n";
    return true;
}
