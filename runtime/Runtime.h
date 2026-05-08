#pragma once

#include <string>

struct RuntimeConfig {
    std::string resolution;
    std::string measureOverlayHotKey;
    std::string minimapOverlayHotkey;
    std::string colorBlind;

    int tolerance = 10;
};

class Runtime{
    public:
        bool initialize();

        const RuntimeConfig& getConfig() const{
            return config;
        };
    private:
        RuntimeConfig config;
};
