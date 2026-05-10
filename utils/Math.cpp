#include "utils/Math.h"

#include <cmath>

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
