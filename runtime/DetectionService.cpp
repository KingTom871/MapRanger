#include "runtime/DetectionService.h"

#include "utils/Math.h"
#include "utils/Profiler.h"

#include <Windows.h>

#include <cmath>
#include <cstddef>
#include <utility>

namespace {
constexpr auto kMiniMapUpdateInterval = std::chrono::milliseconds(500);

std::array<int, 4> calculateMarkerDistances(const RuntimeConfig& config, CaptureService& captureService) {
    std::array<int, 4> distances = {0, 0, 0, 0};
    const MiniMapRegion& scanArea = config.minimapRegion;

    const BGRAImage& image = captureService.captureRegion(
        scanArea.left,
        scanArea.top,
        scanArea.right,
        scanArea.bottom
    );

    ScopedTimer timer("detection");

    const Point& origin = config.defaultOrigin;

    for (int i = 0; i < 4; ++i) {
        const ColorRGB& color = config.markerColors[i];
        const std::vector<Point> markers = Math::detectPingMarkers(image, color, config.tolerance);

        if (markers.empty()) {
            distances[i] = 0;
            continue;
        }

        const Point absoluteMarker{
            markers[0].x + scanArea.left,
            markers[0].y + scanArea.top
        };

        distances[i] = static_cast<int>(
            std::lround(
                Math::calculateDistance(
                    origin,
                    absoluteMarker,
                    config.pixelPer100m
                )
            )
        );
    }

    return distances;
}
}

bool DetectionService::update(
    bool miniMapVisible,
    bool measureVisible,
    const RuntimeConfig& config,
    bool force
) {
    bool changed = false;
    const Clock::time_point now = Clock::now();

    if (miniMapVisible) {
        changed = updateMiniMap(config, now, force) || changed;
    }

    return changed;
}

bool DetectionService::updateCursor(bool measureVisible) {
    if (!measureVisible) {
        return false;
    }

    POINT cursorPos{};
    if (GetCursorPos(&cursorPos) == FALSE) {
        return false;
    }

    const Point nextPoint{cursorPos.x, cursorPos.y};
    if (nextPoint.x == playerPoint.x && nextPoint.y == playerPoint.y) {
        return false;
    }

    playerPoint = nextPoint;
    return true;
}

const MiniMapDetectionResult& DetectionService::getMiniMapResult() const {
    return miniMapResult;
}

const Point& DetectionService::getPlayerPoint() const {
    return playerPoint;
}

bool DetectionService::updateMiniMap(
    const RuntimeConfig& config,
    Clock::time_point now,
    bool force
) {
    if (!force && now < nextMiniMapUpdate) {
        return false;
    }

    nextMiniMapUpdate = now + kMiniMapUpdateInterval;

    const std::array<int, 4> distances = calculateMarkerDistances(config, miniMapCapture);
    if (distances == miniMapResult.distances) {
        return false;
    }

    miniMapResult.distances = distances;
    return true;
}
