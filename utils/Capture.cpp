#include "utils/Capture.h"

#include <windows.h>

BGRAImage Capture::captureRegion(
    int left,
    int top,
    int right,
    int bottom
) {
    BGRAImage img;

    img.width = right - left;
    img.height = bottom - top;

    const int width = img.width;
    const int height = img.height;

    // Allocate pixel buffer (4 bytes per pixel for BGRA)
    img.pixels.resize(width * height * 4);

    HDC hScreen = GetDC(nullptr);
    HDC hMem = CreateCompatibleDC(hScreen);

    HBITMAP hBitmap = CreateCompatibleBitmap(
        hScreen,
        width,
        height
    );

    SelectObject(hMem, hBitmap);

    BitBlt(
        hMem,
        0,
        0,
        width,
        height,
        hScreen,
        left,
        top,
        SRCCOPY
    );

    // Set up BITMAPINFO for GetDIBits
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    GetDIBits(
        hMem,
        hBitmap,
        0,
        height,
        img.pixels.data(),
        &bmi,
        DIB_RGB_COLORS
    );

    DeleteObject(hBitmap);
    DeleteDC(hMem);
    ReleaseDC(nullptr, hScreen);

    return img;
}