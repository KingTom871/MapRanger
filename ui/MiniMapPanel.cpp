#include "ui/MiniMapPanel.h"

#include "ImGui/imgui.h"

#include <cstdio>

namespace UI {
namespace {
ImU32 toImColor(const ColorRGB& color, int alpha = 255) {
    return IM_COL32(color.r, color.g, color.b, alpha);
}

ImVec2 centeredTextPosition(const ImVec2& min, const ImVec2& max, const char* text) {
    const ImVec2 textSize = ImGui::CalcTextSize(text);
    return ImVec2(
        min.x + (max.x - min.x - textSize.x) * 0.5f,
        min.y + (max.y - min.y - textSize.y) * 0.5f
    );
}
}

void renderMiniMapPanel(
    bool enabled,
    const RuntimeConfig& config,
    const std::array<int, 4>& distances
) {
    if (!enabled) {
        return;
    }

    const MiniMapRegion& scanArea = config.minimapRegion;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::Begin("##overlay", nullptr, flags);

    ImDrawList* draw = ImGui::GetWindowDrawList();

    constexpr float borderThickness = 2.0f;
    constexpr float panelHeight = 45.0f;
    constexpr float rounding = 8.0f;
    constexpr float innerGap = 8.0f;

    const ImVec2 mapMin(static_cast<float>(scanArea.left), static_cast<float>(scanArea.top));
    const ImVec2 mapMax(static_cast<float>(scanArea.right), static_cast<float>(scanArea.bottom));
    const ImVec2 panelMin(mapMin.x, mapMin.y - panelHeight - innerGap);
    const ImVec2 panelMax(mapMax.x, mapMin.y - innerGap);

    const ImU32 panelBg = IM_COL32(18, 20, 24, 235);
    const ImU32 panelStroke = IM_COL32(74, 82, 96, 245);
    const ImU32 softStroke = IM_COL32(118, 130, 150, 165);
    const ImU32 textShadow = IM_COL32(6, 8, 12, 220);

    draw->AddRectFilled(panelMin, panelMax, panelBg, rounding);
    draw->AddRect(panelMin, panelMax, panelStroke, rounding, 0, borderThickness);
    draw->AddRect(
        ImVec2(mapMin.x - 1.0f, mapMin.y - 1.0f),
        ImVec2(mapMax.x + 1.0f, mapMax.y + 1.0f),
        softStroke,
        4.0f,
        0,
        borderThickness
    );

    const float width = mapMax.x - mapMin.x;
    const float cellWidth = width / 4.0f;

    for (int i = 0; i < 4; ++i) {
        const float x0 = mapMin.x + cellWidth * static_cast<float>(i);
        const float x1 = (i == 3) ? mapMax.x : x0 + cellWidth;
        const ImVec2 cellMin(x0 + 8.0f, panelMin.y + 8.0f);
        const ImVec2 cellMax(x1 - 8.0f, panelMax.y - 8.0f);

        char text[32];
        sprintf_s(text, "%dm", distances[i]);

        const ImU32 accent = toImColor(config.markerColors[i]);
        draw->AddRectFilled(cellMin, cellMax, IM_COL32(30, 34, 40, 220), 6.0f);
        draw->AddRect(cellMin, cellMax, IM_COL32(72, 80, 94, 180), 6.0f);
        draw->AddCircleFilled(
            ImVec2(cellMin.x + 16.0f, cellMin.y + (cellMax.y - cellMin.y) * 0.5f),
            4.5f,
            accent,
            16
        );

        const ImVec2 textMin(cellMin.x + 22.0f, cellMin.y);
        const ImVec2 textPos = centeredTextPosition(textMin, cellMax, text);
        draw->AddText(ImVec2(textPos.x + 1.0f, textPos.y + 1.0f), textShadow, text);
        draw->AddText(textPos, accent, text);
    }

    ImGui::End();
}

}
