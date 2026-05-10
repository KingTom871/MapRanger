/**
 * @file CommonTypes.h
 * @brief Shared data structures used across modules.
 */

#pragma once

#include <vector>
#include <cstdint>

/**
 * @brief Represents a 2D point.
 */
struct Point
{
    int x = 0;
    int y = 0;
};

/**
 * @brief RGB color.
 */
struct ColorRGB
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

/**
 * @brief BGRA image buffer.
 */
struct BGRAImage
{
    int width = 0;
    int height = 0;
    std::vector<uint8_t> pixels;
};