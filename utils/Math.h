/**
 * @file Math.h
 * @brief Utility class for mathematical operations.
 */
#pragma once

#include "utils/CommonTypes.h"

#include <cstdint>
#include <optional>
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
        static double calculateDistance(
            const Point& p1,
            const Point& p2,
            double scale
        );

        static bool isWhite(
            uint8_t r,
            uint8_t g,
            uint8_t b
        );

        static bool isTargetColor(
            uint8_t r,
            uint8_t g,
            uint8_t b,
            const ColorRGB& target,
            int tolerance
        );

        /**
         * @brief Detects the player marker with a small white center dot.
         *
         * @param image The input image.
         *
         * @return The marker center if found.
         */
        static std::optional<Point> detectPlayerMarker(
            const BGRAImage& image,
            const ColorRGB& targetColor,
            int tolerance
        );

        /**
         * @brief Detects hollow ping markers without a white center.
         *
         * @param image The input image.
         *
         * @return A list of detected ping marker positions.
         */
        static std::vector<Point> detectPingMarkers(
            const BGRAImage& image,
            const ColorRGB& targetColor,
            int tolerance
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
