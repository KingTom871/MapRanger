/**
 * @file DragMeasure.h
 * @brief Drag measure overlay state and rendering for the full-screen map.
 */
#pragma once

#include "runtime/Runtime.h"
#include "utils/CommonTypes.h"

namespace UI {
    struct DragMeasureState {
        bool isDragging = false;
        Point dragStart;
        Point dragCurrent;
    };

    /**
     * @brief Updates drag-measure state based on the current global left mouse button state.
     *
     * @param enabled Whether the drag measure overlay is currently enabled.
     * @return True when the drag state changed and the overlay should be refreshed.
     */
    bool updateDragMeasure(bool enabled);

    /**
     * @brief Renders the current drag measure overlay if a drag is in progress.
     *
     * @param enabled Whether the drag measure overlay is currently enabled.
     * @param config Runtime configuration for pixel-to-meter conversion.
     */
    void renderDragMeasure(
        bool enabled,
        const RuntimeConfig& config
    );

    /**
     * @brief Gets the current drag measure state.
     */
    const DragMeasureState& getDragMeasureState();
}
