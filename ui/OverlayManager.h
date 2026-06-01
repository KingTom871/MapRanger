/**
 * @file OverlayManager.h
 * @brief Minimal visibility and dirty-state tracking for overlay modules.
 */
#pragma once

class OverlayManager {
public:
    bool updateVisibility(bool miniMapEnabled);

    bool isMiniMapVisible() const;
    bool hasVisibleOverlays() const;

    bool isDirty() const;
    void markDirty();
    void clearDirty();

private:
    bool miniMapVisible = true;
    bool dirty = true;
};
