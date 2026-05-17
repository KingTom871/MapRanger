#include "utils/Math.h"

#include <algorithm>
#include <cstddef>
#include <cmath>

namespace {
    constexpr int kBytesPerPixel = 4;

    // Checks if the image has a valid size and pixel data.
    bool hasUsablePixels(const BGRAImage& image)
    {
        if (image.width <= 0 || image.height <= 0) {
            return false;
        }

        const int expectedSize = image.width * image.height * kBytesPerPixel;
        return image.pixels.size() >= static_cast<std::size_t>(expectedSize);
    }

    void readPixel(
        const BGRAImage& image,
        int x,
        int y,
        uint8_t& r,
        uint8_t& g,
        uint8_t& b
    ) {
        const int index = (y * image.width + x) * kBytesPerPixel;
        b = image.pixels[index + 0];
        g = image.pixels[index + 1];
        r = image.pixels[index + 2];
    }

    // Checks if the given (x, y) coordinate is within a certain radius of any point in the list.
    bool isNearExistingPoint(
        const std::vector<Point>& points,
        int x,
        int y,
        int radius
    ) {
        const int radiusSq = radius * radius;

        for (const Point& point : points) {
            const int dx = point.x - x;
            const int dy = point.y - y;

            if (dx * dx + dy * dy <= radiusSq) {
                return true;
            }
        }

        return false;
    }

    int countQuadrants(int mask)
    {
        int count = 0;

        while (mask != 0) {
            count += mask & 1;
            mask >>= 1;
        }

        return count;
    }
}

// Calculates the distance between two points in 2D space, given a pixel-to-meter scale.
double Math::calculateDistance(
    const Point& p1,
    const Point& p2,
    double scale
) {
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    float distanceInPixels = std::sqrt(dx * dx + dy * dy);

    return distanceInPixels * (100.0 / scale);
}

bool Math::isWhite(
    uint8_t r,
    uint8_t g,
    uint8_t b
) {
    return r >= 250 && g >= 235 && b >= 180;
}

bool Math::isTargetColor(
    uint8_t r,
    uint8_t g,
    uint8_t b,
    const ColorRGB& target,
    int tolerance
) {
    // return r >= 195 && g >= 190 && b <= 20 && (r - b) >= 130 && (g - b) >= 90;
        return (
        std::abs(r - target.r) <= tolerance &&
        std::abs(g - target.g) <= tolerance &&
        std::abs(b - target.b) <= tolerance
    );
}

// Detects the player marker with white pixels.
std::optional<Point> Math::detectPlayerMarker(
    const BGRAImage& image,
    const ColorRGB& targetColor,
    int tolerance
) {
    if (!hasUsablePixels(image)) {
        return std::nullopt;
    }

    constexpr int inspectRadius = 15;
    constexpr int innerRadius = 4;
    constexpr int minNearbyTargetColor = 18;

    for (int y = inspectRadius; y < image.height - inspectRadius; ++y) {
        for (int x = inspectRadius; x < image.width - inspectRadius; ++x) {
            uint8_t r = 0;
            uint8_t g = 0;
            uint8_t b = 0;
            readPixel(image, x, y, r, g, b);

            if (!isWhite(r, g, b)) {
                continue;
            }

            int targetColorCount = 0;
            int nearbyWhiteCount = 0;

            for (int dy = -inspectRadius; dy <= inspectRadius; ++dy) {
                for (int dx = -inspectRadius; dx <= inspectRadius; ++dx) {
                    const int distanceSq = dx * dx + dy * dy;

                    if (distanceSq > inspectRadius * inspectRadius) {
                        continue;
                    }

                    readPixel(image, x + dx, y + dy, r, g, b);

                    if (distanceSq <= innerRadius * innerRadius && isWhite(r, g, b)) {
                        ++nearbyWhiteCount;
                    } else if (distanceSq > innerRadius * innerRadius && isTargetColor(r, g, b, targetColor, tolerance)) {
                        ++targetColorCount;
                    }
                }
            }

            if (targetColorCount >= minNearbyTargetColor && nearbyWhiteCount > 0) {
                return Point{ x, y };
            }
        }
    }

    return std::nullopt;
}

// Detects hollow ping markers without a white center.
std::vector<Point> Math::detectPingMarkers(
    const BGRAImage& image,
    const ColorRGB& targetColor,
    int tolerance
) {
    std::vector<Point> results;

    if (!hasUsablePixels(image)) {
        return results;
    }

    constexpr int inspectRadius = 15;
    constexpr int centerRadius = 4;
    constexpr int minTargetColorPixels = 30;
    constexpr int maxCenterTargetColorPixels = 2;
    constexpr int duplicateRadius = 18;

    for (int y = inspectRadius; y < image.height - inspectRadius; ++y) {
        for (int x = inspectRadius; x < image.width - inspectRadius; ++x) {
            uint8_t r = 0;
            uint8_t g = 0;
            uint8_t b = 0;
            readPixel(image, x, y, r, g, b);

            if (!isTargetColor(r, g, b, targetColor, tolerance) || isNearExistingPoint(results, x, y, duplicateRadius)) {
                continue;
            }

            int targetColorCount = 0;
            int minTargetColorX = x;
            int maxTargetColorX = x;
            int minTargetColorY = y;
            int maxTargetColorY = y;
            long long sumX = 0;
            long long sumY = 0;

            for (int dy = -inspectRadius; dy <= inspectRadius; ++dy) {
                for (int dx = -inspectRadius; dx <= inspectRadius; ++dx) {
                    const int px = x + dx;
                    const int py = y + dy;
                    const int distanceSq = dx * dx + dy * dy;

                    if (distanceSq > inspectRadius * inspectRadius) {
                        continue;
                    }

                    readPixel(image, px, py, r, g, b);

                    if (!isTargetColor(r, g, b, targetColor, tolerance)) {
                        continue;
                    }

                    ++targetColorCount;
                    sumX += px;
                    sumY += py;
                    minTargetColorX = std::min(minTargetColorX, px);
                    maxTargetColorX = std::max(maxTargetColorX, px);
                    minTargetColorY = std::min(minTargetColorY, py);
                    maxTargetColorY = std::max(maxTargetColorY, py);
                }
            }

            if (targetColorCount < minTargetColorPixels) {
                continue;
            }

            const Point center{
                static_cast<int>(sumX / targetColorCount),
                static_cast<int>(sumY / targetColorCount)
            };

            if (
                center.x < centerRadius ||
                center.y < centerRadius ||
                center.x >= image.width - centerRadius ||
                center.y >= image.height - centerRadius ||
                isNearExistingPoint(results, center.x, center.y, duplicateRadius)
            ) {
                continue;
            }

            int centerTargetColorCount = 0;
            int centerWhiteCount = 0;
            int quadrantMask = 0;

            for (int dy = -inspectRadius; dy <= inspectRadius; ++dy) {
                for (int dx = -inspectRadius; dx <= inspectRadius; ++dx) {
                    const int px = center.x + dx;
                    const int py = center.y + dy;
                    const int distanceSq = dx * dx + dy * dy;

                    if (
                        px < 0 ||
                        py < 0 ||
                        px >= image.width ||
                        py >= image.height ||
                        distanceSq > inspectRadius * inspectRadius
                    ) {
                        continue;
                    }

                    readPixel(image, px, py, r, g, b);

                    if (distanceSq <= centerRadius * centerRadius) {
                        if (isWhite(r, g, b)) {
                            ++centerWhiteCount;
                        }

                        if (isTargetColor(r, g, b, targetColor, tolerance)) {
                            ++centerTargetColorCount;
                        }
                    }

                    if (!isTargetColor(r, g, b, targetColor, tolerance)) {
                        continue;
                    }

                    if (dx < 0 && dy < 0) {
                        quadrantMask |= 1;
                    } else if (dx >= 0 && dy < 0) {
                        quadrantMask |= 2;
                    } else if (dx < 0 && dy >= 0) {
                        quadrantMask |= 4;
                    } else {
                        quadrantMask |= 8;
                    }
                }
            }

            const int spanX = maxTargetColorX - minTargetColorX;
            const int spanY = maxTargetColorY - minTargetColorY;
            const bool hasShapeAroundCenter =
                targetColorCount >= minTargetColorPixels &&
                countQuadrants(quadrantMask) >= 3 &&
                spanX >= 5 &&
                spanY >= 5;

            if (
                hasShapeAroundCenter &&
                centerTargetColorCount <= maxCenterTargetColorPixels &&
                centerWhiteCount == 0
            ) {
                results.push_back(center);
            }
        }
    }

    return results;
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
