/**
 * @brief Calculates the distance between two points in 2D space.
 * 
 * @param p1 The first point.
 * @param p2 The second point.
 * @param scale scale Pixel-to-meter scale.
 * 
 * @return The distance between the two points in meters.
 */
#pragma once

// represents a point in 2D space
struct Point{
    int x;
    int y;
};

// 
class Math{
    public:
        static float calculateDistance(
            const Point& p1,
            const Point& p2,
            float scale
        );
};
