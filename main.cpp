#include "runtime/Runtime.h"
#include "runtime/DetectionService.h"
#include "utils/Input.h"
#include "utils/Profiler.h"
#include "graphics/GraphicsContext.h"
#include "ui/OverlayManager.h"
#include "ui/MiniMapPanel.h"
#include "ui/DragMeasure.h"

#include <iostream>

int main() {
    SetProcessDPIAware();

    Runtime runtime;

    if(!runtime.initialize()) {
        std::cerr << "[Runtime] Failed to initialize runtime" << std::endl;
        return 1;
    }

    const RuntimeConfig& config = runtime.getConfig();

    std::cout << "[Runtime] Resolution: " << config.resolution << std::endl;

    Input input(config);
    GraphicsContext graphics;
    DetectionService detection;
    OverlayManager overlays;

    if (!graphics.initialize(L"MapRanger")) {
        std::cerr << "[Graphics] Failed to initialize graphics\n";
        return 1;
    }

    if (!input.initialize(graphics.getHwnd())) {
        std::cerr << "[Input] Failed to initialize input\n";
    }

    MSG msg{};
    bool running = true;

    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }

            input.processMessage(msg);

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running) {
            break;
        }

        const bool visibilityChanged = overlays.updateVisibility(
            input.isMiniMapEnabled(),
            input.isMeasureEnabled()
        );
        const bool hasVisibleOverlays = overlays.hasVisibleOverlays();

        graphics.setWindowVisible(hasVisibleOverlays);

        if (!hasVisibleOverlays) {
            overlays.clearDirty();
            Profiler::instance().printIfDue();
            MsgWaitForMultipleObjects(0, nullptr, FALSE, 100, QS_ALLINPUT);
            continue;
        }

        if (detection.update(
                overlays.isMiniMapVisible(),
                overlays.isMeasureVisible(),
                config,
                visibilityChanged
            )) {
            overlays.markDirty();
        }

        if (detection.updateCursor(overlays.isMeasureVisible())) {
            overlays.markDirty();
        }

        if (UI::updateDragMeasure(overlays.isMeasureVisible())) {
            overlays.markDirty();
        }

        if (overlays.isDirty()) {
            graphics.beginFrame();
            UI::renderMiniMapPanel(
                overlays.isMiniMapVisible(),
                config,
                detection.getMiniMapResult().distances
            );
            UI::renderDragMeasure(
                overlays.isMeasureVisible(),
                config
            );
            graphics.endFrame();
            overlays.clearDirty();
        }

        Profiler::instance().printIfDue();
        MsgWaitForMultipleObjects(0, nullptr, FALSE, 16, QS_ALLINPUT);
    }
    
    return 0;
}
