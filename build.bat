@echo off

if not exist "build" (
    mkdir build
)

if not exist "build\obj" (
    mkdir build\obj
)

cl ^
/std:c++20 ^
/EHsc ^
/I. ^
/Iincludes ^
main.cpp ^
runtime/Runtime.cpp ^
utils/Math.cpp ^
/Fobuild\obj\ ^
/Febuild\MapRanger.exe ^
user32.lib ^
gdi32.lib

if %ERRORLEVEL% equ 0 (
    echo Build successful.
) else (
    echo Build failed with error code %ERRORLEVEL%.
)
