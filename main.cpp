#include "runtime/Runtime.h"

#include <iostream>

int main() {
    Runtime runtime;

    if(!runtime.initialize()) {
        std::cerr << "Failed to initialize runtime" << std::endl;
        return 1;
    }

    const RuntimeConfig& config = runtime.getConfig();

    std::cout << "Resolution: " << config.resolution << std::endl;
    std::cout << "Measure Overlay Hotkey: " << config.measureOverlayHotKey << std::endl;
    std::cout << "Minimap Overlay Hotkey: " << config.minimapOverlayHotkey << std::endl;
    std::cout << "Color Blind Mode: " << config.colorBlind << std::endl;
    std::cout << "Tolerance: " << config.tolerance << std::endl;

    std::cin.get();
    
    return 0;
}
