/**
 * @file MiniMapPanel.h
 * @brief Dear ImGui panel for the minimap overlay.
 */
#pragma once

#include "runtime/Runtime.h"

#include <array>

namespace UI {
    /**
     * @brief Renders the minimap overlay panel.
     *
     * @param enabled Whether the minimap overlay should be visible.
     * @param config Runtime configuration used for minimap bounds and colors.
     * @param distances Precomputed marker distances.
     */
    void renderMiniMapPanel(
        bool enabled,
        const RuntimeConfig& config,
        const std::array<int, 4>& distances
    );
}
