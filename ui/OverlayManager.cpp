#include "ui/OverlayManager.h"

bool OverlayManager::updateVisibility(bool miniMapEnabled, bool measureEnabled) {
    const bool changed =
        miniMapVisible != miniMapEnabled ||
        measureVisible != measureEnabled;

    miniMapVisible = miniMapEnabled;
    measureVisible = measureEnabled;

    if (changed) {
        dirty = true;
    }

    return changed;
}

bool OverlayManager::isMiniMapVisible() const {
    return miniMapVisible;
}

bool OverlayManager::isMeasureVisible() const {
    return measureVisible;
}

bool OverlayManager::hasVisibleOverlays() const {
    return miniMapVisible || measureVisible;
}

bool OverlayManager::isDirty() const {
    return dirty;
}

void OverlayManager::markDirty() {
    dirty = true;
}

void OverlayManager::clearDirty() {
    dirty = false;
}
