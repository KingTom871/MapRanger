#pragma once

#include "utils/CommonTypes.h"

#include <array>
#include <string>

/**
 * @brief Minimap region.
 */
struct MiniMapRegion {
    int left;
    int top;
    int right;
    int bottom;
};

/**
 * @brief Runtime configuration.
 */
struct RuntimeConfig {
    std::string resolution;
    std::string measureOverlayHotKey;
    std::string minimapOverlayHotkey;
    std::string colorBlind;
    MiniMapRegion minimapRegion;
    std::array<ColorRGB, 4> markerColors = {
        ColorRGB{233, 229, 17},
        ColorRGB{218, 98, 38},
        ColorRGB{58, 160, 217},
        ColorRGB{68, 181, 73}
    };
    Point defaultOrigin;

    double pixelPer100m = 87.5;
    int tolerance = 10;
};


class Runtime{
    public:
        /**
         * @brief Initializes runtime settings from config.ini and the current display.
         */
        bool initialize();

        /**
         * @brief Gets the loaded runtime configuration.
         */
        const RuntimeConfig& getConfig() const{
            return config;
        };
    private:
        RuntimeConfig config;
};
