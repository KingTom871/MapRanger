#include "ui/FullMapAutoMeasure.h"
#include "utils/Capture.h"
#include "utils/Math.h"

#include <ImGui/imgui.h>

#include <Windows.h>

#include <vector>
#include <string>

namespace UI {

void renderFullMapAutoMeasure(
    bool enabled,
    const RuntimeConfig& config
) {
    if(!enabled){
        return;
    }

    // Capture full map region
    BGRAImage image = Capture::captureRegion(
        0,
        0,
        config.fullScreenRight,
        config.fullScreenBottom
    );

    std::vector<Point> pingPoints;

    // Detect ping markers
    for(const auto& color : config.markerColors){

        std::vector<Point> pings =
            Math::detectPingMarkers(
                image,
                color,
                config.tolerance
            );

        pingPoints.insert(
            pingPoints.end(),
            pings.begin(),
            pings.end()
        );
    }

    if(pingPoints.empty()){
        return;
    }

    // Get current mouse position
    POINT cursorPos{};

    GetCursorPos(&cursorPos);

    Point playerPoint{
        cursorPos.x,
        cursorPos.y
    };

    ImDrawList* drawList =
        ImGui::GetForegroundDrawList();

    constexpr float boxSize = 30.0f;
    constexpr float halfBox = boxSize * 0.5f;

    // Draw ping boxes
    for(const auto& point : pingPoints){

        float screenX =
            static_cast<float>(point.x);

        float screenY =
            static_cast<float>(point.y);

        drawList->AddRect(
            ImVec2(
                screenX - halfBox,
                screenY - halfBox
            ),
            ImVec2(
                screenX + halfBox,
                screenY + halfBox
            ),
            IM_COL32(255, 255, 0, 255),
            0.0f,
            0,
            2.0f
        );
    }

    // Draw lines from mouse -> ping
    for(const auto& ping : pingPoints){

        float x1 =
            static_cast<float>(playerPoint.x);

        float y1 =
            static_cast<float>(playerPoint.y);

        float x2 =
            static_cast<float>(ping.x);

        float y2 =
            static_cast<float>(ping.y);

        // Draw line
        drawList->AddLine(
            ImVec2(x1, y1),
            ImVec2(x2, y2),
            IM_COL32(0, 255, 255, 255),
            2.0f
        );

        // Calculate distance
        double distance =
            Math::calculateDistance(
                playerPoint,
                ping,
                config.pixelPer100m
            );

        std::string text =
            std::to_string(
                static_cast<int>(distance)
            ) + "m";

        // Midpoint
        float midX = (x1 + x2) * 0.5f;

        float midY = (y1 + y2) * 0.5f;

        ImVec2 textSize =
            ImGui::CalcTextSize(text.c_str());

        // Background
        drawList->AddRectFilled(
            ImVec2(
                midX - textSize.x * 0.5f - 4.0f,
                midY - textSize.y * 0.5f - 2.0f
            ),
            ImVec2(
                midX + textSize.x * 0.5f + 4.0f,
                midY + textSize.y * 0.5f + 2.0f
            ),
            IM_COL32(0, 0, 0, 180)
        );

        // Distance text
        drawList->AddText(
            ImVec2(
                midX - textSize.x * 0.5f,
                midY - textSize.y * 0.5f
            ),
            IM_COL32(255, 255, 255, 255),
            text.c_str()
        );
    }
}

}