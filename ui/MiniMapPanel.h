/**
 * @file MiniMapPanel.h
 * @brief Dear ImGui panel for the minimap overlay.
 */
#pragma once

#include "runtime/Runtime.h"

namespace UI {
    /**
     * @brief Renders the minimap overlay panel.
     *
     * @param enabled Whether the minimap overlay should be visible.
     * @param config Runtime configuration used for minimap bounds and colors.
     */
    void renderMiniMapPanel(bool enabled, const RuntimeConfig& config);
}
