/**
 * @file Input.h
 * @brief Win32 hotkey input handling and overlay state toggling.
 */
#pragma once

#include "runtime/Runtime.h"

#include <Windows.h>

/**
 * @brief Handles global hotkey registration and overlay visibility states.
 */
class Input {
public:
    /**
     * @brief Creates an input manager using runtime hotkey configuration.
     *
     * @param config Runtime configuration containing overlay hotkey strings.
     */
    explicit Input(const RuntimeConfig& config);

    /**
     * @brief Releases registered global hotkeys.
     */
    ~Input();

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    /**
     * @brief Registers configured global hotkeys.
     *
     * @param hwnd Window handle that receives WM_HOTKEY messages.
     *
     * @return True when all configured hotkeys were parsed and registered.
     */
    bool initialize(HWND hwnd);

    /**
     * @brief Processes Win32 messages related to registered hotkeys.
     *
     * @param msg Win32 message from the application message loop.
     */
    void processMessage(const MSG& msg);

    /**
     * @brief Checks whether the MiniMap overlay is enabled.
     *
     * @return True when the MiniMap overlay should be visible.
     */
    bool isMiniMapEnabled() const;

    /**
     * @brief Checks whether the drag measure overlay is enabled.
     *
     * @return True when the drag measure overlay should be visible.
     */
    bool isMeasureEnabled() const;

private:
    static constexpr int MiniMapHotkeyId = 1;
    static constexpr int MeasureHotkeyId = 2;

    const RuntimeConfig& config;
    HWND hwnd = nullptr;
    bool minimapEnabled = true;
    bool measureEnabled = true;
    bool miniMapHotkeyRegistered = false;
    bool measureHotkeyRegistered = false;

    void unregisterHotkeys();
};
