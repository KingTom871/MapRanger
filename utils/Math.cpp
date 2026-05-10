#include "utils/Math.h"

#include <cmath>

// Calculates the distance between two points in 2D space, given a pixel-to-meter scale.
float Math::calculateDistance(
    const Point& p1,
    const Point& p2,
    float scale
) {
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    float distanceInPixels = std::sqrt(dx * dx + dy * dy);
    return distanceInPixels * scale; // convert to meters
}

// Detects markers in a BGRA image based on specified colors and tolerance.
std::vector<Point> Math::detectMarkers(
    const BGRAImage& image,
    const std::vector<ColorRGB>& colors,
    int tolerance
) {
    std::vector<Point> results;

    for (const ColorRGB& color : colors) {
        long long sumX = 0;
        long long sumY = 0;

        int count = 0;

        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                const int index =
                    (y * image.width + x) * 4;

                const uint8_t b =
                    image.pixels[index + 0];

                const uint8_t g =
                    image.pixels[index + 1];

                const uint8_t r =
                    image.pixels[index + 2];

                if (
                    std::abs(r - color.r) <= tolerance &&
                    std::abs(g - color.g) <= tolerance &&
                    std::abs(b - color.b) <= tolerance
                ){
                    sumX += x;
                    sumY += y;

                    count++;
                }
            }
        }

        
        if (count > 0) {
            Point center;
            center.x = sumX / count;
            center.y = sumY / count;
            results.push_back(center);
        }
    }

    return results;
}