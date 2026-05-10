/**
 * @file Capture.h
 * @brief Utility class for capturing the screen using Win32 GDI.
 */

#pragma once

#include "utils/CommonTypes.h"

#include <vector>
#include <cstdint>

class Capture {
    public:
        /**
         * @brief Captures a region of the screen.
         * 
         * @param left The left coordinate of the region.
         * @param top The top coordinate of the region.
         * @param right The right coordinate of the region.
         * @param bottom The bottom coordinate of the region.
         * 
         * @return BGRA image buffer.
         */
        static BGRAImage captureRegion(
            int left,
            int top,
            int right,
            int bottom
        );
};