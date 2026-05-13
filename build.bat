@echo off

if not exist "build" (
    mkdir build
)

if not exist "build\obj" (
    mkdir build\obj
)

set sourceFiles=main.cpp ^
runtime\runtime.cpp ^
utils\Math.cpp ^
utils\Capture.cpp ^
utils\Input.cpp ^
graphics\GraphicsContext.cpp ^
includes\ImGui\imgui.cpp ^
includes\ImGui\imgui_draw.cpp ^
includes\ImGui\imgui_tables.cpp ^
includes\ImGui\imgui_widgets.cpp ^
includes\ImGui\backends\imgui_impl_win32.cpp ^
includes\ImGui\backends\imgui_impl_dx11.cpp

cl /std:c++20 /EHsc /I. /Iincludes /I includes/ImGui %sourceFiles% ^
/Fobuild\obj\ ^
/Febuild\MapRanger.exe ^
user32.lib gdi32.lib d3d11.lib dxgi.lib imm32.lib

if %ERRORLEVEL% equ 0 (
    echo Build successful.
) else (
    echo Build failed with error code %ERRORLEVEL%.
)
