#include "ui/OverlayManager.h"

bool OverlayManager::updateVisibility(bool miniMapEnabled) {
    const bool changed = miniMapVisible != miniMapEnabled;

    miniMapVisible = miniMapEnabled;

    if (changed) {
        dirty = true;
    }

    return changed;
}

bool OverlayManager::isMiniMapVisible() const {
    return miniMapVisible;
}

bool OverlayManager::hasVisibleOverlays() const {
    return miniMapVisible;
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
