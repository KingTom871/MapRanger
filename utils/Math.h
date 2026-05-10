/**
 * @file Math.h
 * @brief Utility class for mathematical operations.
 */
#pragma once

#include "utils/CommonTypes.h"

#include <cstdint>
#include <vector>

class Math{
    public:
        /**
         * @brief Calculates the distance between two points in 2D space.
         * 
         * @param p1 The first point.
         * @param p2 The second point.
         * @param scale scale Pixel-to-meter scale.
         * 
         * @return The distance between the two points in meters.
         */
        static float calculateDistance(
            const Point& p1,
            const Point& p2,
            float scale
        );

        /**
         * @brief Detects markers in a BGRA image.
         * 
         * @param image The input image.
         * @param markerColors The list of colors to detect.
         * @param tolerance The tolerance for color matching.
         * 
         * @return A list of detected marker positions.
         */
        static std::vector<Point> detectMarkers(
            const BGRAImage& image,
            const std::vector<ColorRGB>& markerColors,
            int tolerance
        );
};
