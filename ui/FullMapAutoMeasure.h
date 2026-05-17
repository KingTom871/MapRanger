/**
 * @file FullMapAutoMeasure.h
 * @brief Auto detect and measure pings on the full map.
 */
#pragma once

#include "runtime/Runtime.h"

namespace UI {
    /**
     * @brief Renders the full map auto-measurement ui.
     *
     * @param enabled Whether the full map auto-measurement should be active.
     * @param config Runtime configuration used for measurement parameters.
     */
    void renderFullMapAutoMeasure(bool enabled, const RuntimeConfig& config);
}