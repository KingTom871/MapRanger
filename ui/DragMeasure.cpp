#include "ui/DragMeasure.h"

#include "utils/Math.h"

#include <ImGui/imgui.h>
#include <Windows.h>
#include <cmath>
#include <cstdio>

namespace UI {
namespace {
    DragMeasureState state;
    bool previousLeftButtonDown = false;

    constexpr ImU32 kLineColor = IM_COL32(255, 160, 55, 230);
    constexpr ImU32 kPointColor = IM_COL32(240, 210, 120, 255);
    constexpr ImU32 kTextColor = IM_COL32(255, 255, 255, 230);
    constexpr ImU32 kTextShadowColor = IM_COL32(0, 0, 0, 200);
    constexpr ImU32 kTextBackground = IM_COL32(12, 16, 24, 210);
    constexpr float kLineThickness = 2.0f;
    constexpr float kPointRadius = 5.0f;
    constexpr float kTextPaddingX = 8.0f;
    constexpr float kTextPaddingY = 4.0f;
    constexpr float kTextRounding = 4.0f;

    bool isLeftButtonDown() {
        return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    }
}

bool updateDragMeasure(bool enabled) {
    bool changed = false;
    const bool leftDown = isLeftButtonDown();
    POINT cursorPos{};

    if (GetCursorPos(&cursorPos) == FALSE) {
        if (!enabled && state.isDragging) {
            state.isDragging = false;
            changed = true;
        }
        previousLeftButtonDown = leftDown;
        return changed;
    }

    const Point currentPoint{cursorPos.x, cursorPos.y};

    if (!enabled) {
        if (state.isDragging) {
            state.isDragging = false;
            changed = true;
        }
        previousLeftButtonDown = leftDown;
        return changed;
    }

    if (leftDown && !previousLeftButtonDown) {
        state.isDragging = true;
        state.dragStart = currentPoint;
        state.dragCurrent = currentPoint;
        changed = true;
    } else if (state.isDragging && leftDown) {
        if (currentPoint.x != state.dragCurrent.x || currentPoint.y != state.dragCurrent.y) {
            state.dragCurrent = currentPoint;
            changed = true;
        }
    } else if (state.isDragging && !leftDown) {
        state.isDragging = false;
        changed = true;
    }

    previousLeftButtonDown = leftDown;
    return changed;
}

void renderDragMeasure(
    bool enabled,
    const RuntimeConfig& config
) {
    if (!enabled || !state.isDragging) {
        return;
    }

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    const ImVec2 start(static_cast<float>(state.dragStart.x), static_cast<float>(state.dragStart.y));
    const ImVec2 end(static_cast<float>(state.dragCurrent.x), static_cast<float>(state.dragCurrent.y));

    drawList->AddLine(start, end, kLineColor, kLineThickness);
    drawList->AddCircleFilled(start, kPointRadius, kPointColor, 16);
    drawList->AddCircleFilled(end, kPointRadius, kPointColor, 16);

    const double distanceMeters = Math::calculateDistance(
        state.dragStart,
        state.dragCurrent,
        config.pixelPer100m
    );

    char buffer[32];
    sprintf_s(buffer, "%dm", static_cast<int>(std::lround(distanceMeters)));

    const ImVec2 textSize = ImGui::CalcTextSize(buffer);
    const ImVec2 midPoint(
        (start.x + end.x) * 0.5f,
        (start.y + end.y) * 0.5f
    );
    const ImVec2 textPos(
        midPoint.x - textSize.x * 0.5f,
        midPoint.y - textSize.y * 0.5f
    );
    const ImVec2 bgMin(textPos.x - kTextPaddingX, textPos.y - kTextPaddingY);
    const ImVec2 bgMax(textPos.x + textSize.x + kTextPaddingX, textPos.y + textSize.y + kTextPaddingY);

    drawList->AddRectFilled(bgMin, bgMax, kTextBackground, kTextRounding);
    drawList->AddText(ImVec2(textPos.x + 1.0f, textPos.y + 1.0f), kTextShadowColor, buffer);
    drawList->AddText(textPos, kTextColor, buffer);
}

const DragMeasureState& getDragMeasureState() {
    return state;
}
}
