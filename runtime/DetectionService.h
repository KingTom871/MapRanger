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
        const RuntimeConfig& config,
        bool force
    );

    const MiniMapDetectionResult& getMiniMapResult() const;

private:
    using Clock = std::chrono::steady_clock;

    bool updateMiniMap(const RuntimeConfig& config, Clock::time_point now, bool force);

    CaptureService miniMapCapture;
    MiniMapDetectionResult miniMapResult;
    Clock::time_point nextMiniMapUpdate{};
};
