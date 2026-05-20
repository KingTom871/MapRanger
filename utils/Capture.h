/**
 * @file Capture.h
 * @brief Utility class for capturing the screen using Win32 GDI.
 */

#pragma once

#include "utils/CommonTypes.h"

#include <Windows.h>

#include <vector>
#include <cstdint>

/**
 * @brief Reuses Win32 GDI capture resources for repeated region captures.
 */
class CaptureService {
public:
    CaptureService() = default;
    ~CaptureService();

    CaptureService(const CaptureService&) = delete;
    CaptureService& operator=(const CaptureService&) = delete;

    /**
     * @brief Captures a region of the screen into an internally reused BGRA buffer.
     */
    const BGRAImage& captureRegion(
        int left,
        int top,
        int right,
        int bottom
    );

    /**
     * @brief Releases cached GDI resources.
     */
    void reset();

private:
    bool ensureResources(int width, int height);

    HDC screenDc = nullptr;
    HDC memoryDc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    int cachedWidth = 0;
    int cachedHeight = 0;
    BITMAPINFO bitmapInfo{};
    BGRAImage image;
};

class Capture {
    public:
        /**
         * @brief Captures a region of the screen.
         * 
         * @param left The left coordinate of the region.
         * @param top The top coordinate of the region.
         * @param right The right coordinate of the region.
         * @param bottom The bottom coordinate of the region.
         * 
         * @return BGRA image buffer.
         */
        static BGRAImage captureRegion(
            int left,
            int top,
            int right,
            int bottom
        );
};
