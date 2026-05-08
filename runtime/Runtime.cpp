#include "runtime/Runtime.h"

#include "mINI/ini.h"

#include <iostream>

bool Runtime::initialize() {
    std::cout << "Loading config...\n";

    mINI::INIFile file("config.ini");
    mINI::INIStructure ini;

    if(!file.read(ini)) {
        std::cerr << "Failed to read config.ini" << std::endl;
        return false;
    }

    config.resolution = ini["CONFIG"]["resolution"];
    config.measureOverlayHotKey = ini["CONFIG"]["measureOverlayHotKey"];
    config.minimapOverlayHotkey = ini["CONFIG"]["minimapOverlayHotkey"];
    config.colorBlind = ini["CONFIG"]["colorBlind"];
    config.tolerance = std::stoi(ini["CONFIG"]["tolerance"]);

    std::cout << "Runtime initialized\n";
    return true;
}
