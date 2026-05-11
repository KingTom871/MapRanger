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
utils\Input.cpp

cl /std:c++20 /EHsc /I. /Iincludes %sourceFiles% ^
/Fobuild\obj\ ^
/Febuild\MapRanger.exe ^
user32.lib gdi32.lib

if %ERRORLEVEL% equ 0 (
    echo Build successful.
) else (
    echo Build failed with error code %ERRORLEVEL%.
)
