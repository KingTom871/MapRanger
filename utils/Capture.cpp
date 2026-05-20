#include "utils/Capture.h"
#include "utils/Profiler.h"

#include <windows.h>

#include <cstddef>

CaptureService::~CaptureService() {
    reset();
}

const BGRAImage& CaptureService::captureRegion(
    int left,
    int top,
    int right,
    int bottom
) {
    ScopedTimer timer("capture");

    image.width = right - left;
    image.height = bottom - top;

    const int width = image.width;
    const int height = image.height;

    if (width <= 0 || height <= 0 || !ensureResources(width, height)) {
        image.width = 0;
        image.height = 0;
        image.pixels.clear();
        return image;
    }

    BitBlt(
        memoryDc,
        0,
        0,
        width,
        height,
        screenDc,
        left,
        top,
        SRCCOPY
    );

    GetDIBits(
        memoryDc,
        bitmap,
        0,
        height,
        image.pixels.data(),
        &bitmapInfo,
        DIB_RGB_COLORS
    );

    return image;
}

void CaptureService::reset() {
    if (memoryDc != nullptr && oldBitmap != nullptr) {
        SelectObject(memoryDc, oldBitmap);
        oldBitmap = nullptr;
    }

    if (bitmap != nullptr) {
        DeleteObject(bitmap);
        bitmap = nullptr;
    }

    if (memoryDc != nullptr) {
        DeleteDC(memoryDc);
        memoryDc = nullptr;
    }

    if (screenDc != nullptr) {
        ReleaseDC(nullptr, screenDc);
        screenDc = nullptr;
    }

    cachedWidth = 0;
    cachedHeight = 0;
}

bool CaptureService::ensureResources(int width, int height) {
    if (screenDc == nullptr) {
        screenDc = GetDC(nullptr);
        if (screenDc == nullptr) {
            return false;
        }
    }

    if (memoryDc == nullptr) {
        memoryDc = CreateCompatibleDC(screenDc);
        if (memoryDc == nullptr) {
            return false;
        }
    }

    if (bitmap == nullptr || cachedWidth != width || cachedHeight != height) {
        if (memoryDc != nullptr && oldBitmap != nullptr) {
            SelectObject(memoryDc, oldBitmap);
            oldBitmap = nullptr;
        }

        if (bitmap != nullptr) {
            DeleteObject(bitmap);
            bitmap = nullptr;
        }

        bitmap = CreateCompatibleBitmap(screenDc, width, height);
        if (bitmap == nullptr) {
            cachedWidth = 0;
            cachedHeight = 0;
            return false;
        }

        oldBitmap = SelectObject(memoryDc, bitmap);
        cachedWidth = width;
        cachedHeight = height;

        bitmapInfo = {};
        bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmapInfo.bmiHeader.biWidth = width;
        bitmapInfo.bmiHeader.biHeight = -height;
        bitmapInfo.bmiHeader.biPlanes = 1;
        bitmapInfo.bmiHeader.biBitCount = 32;
        bitmapInfo.bmiHeader.biCompression = BI_RGB;
    }

    const std::size_t requiredBytes = static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4;
    if (image.pixels.size() != requiredBytes) {
        image.pixels.resize(requiredBytes);
    }

    return true;
}

BGRAImage Capture::captureRegion(
    int left,
    int top,
    int right,
    int bottom
) {
    static thread_local CaptureService service;
    return service.captureRegion(left, top, right, bottom);
}
