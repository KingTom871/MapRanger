/**
 * @file DetectionService.h
 * @brief Scheduled capture and marker detection for overlay modules.
 */
#pragma once

#include "runtime/Runtime.h"
#include "utils/Capture.h"

#include <array>
#include <chrono>
#include <vector>

struct MiniMapDetectionResult {
    std::array<int, 4> distances = {0, 0, 0, 0};
};

class DetectionService {
public:
    /**
     * @brief Updates visible overlay data when scheduled.
     *
     * @return True when data changed and a redraw is needed.
     */
    bool update(
        bool miniMapVisible,
        bool measureVisible,
        const RuntimeConfig& config,
        bool force
    );

    /**
     * @brief Refreshes cursor-dependent measure data without recapturing the screen.
     */
    bool updateCursor(bool measureVisible);

    const MiniMapDetectionResult& getMiniMapResult() const;
    const Point& getPlayerPoint() const;

private:
    using Clock = std::chrono::steady_clock;

    bool updateMiniMap(const RuntimeConfig& config, Clock::time_point now, bool force);

    CaptureService miniMapCapture;
    MiniMapDetectionResult miniMapResult;
    Point playerPoint{};
    Clock::time_point nextMiniMapUpdate{};
};
