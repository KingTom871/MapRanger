#pragma once

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

    double pixelToMeterScale = 87.5;
    int tolerance = 10;
};


class Runtime{
    public:
        /**
         * @brief Initializes the runtime by loading the configuration from config.ini.
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
