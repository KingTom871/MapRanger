@echo off

if not exist "build" (
    mkdir build
)

cl /std:c++20 /EHsc main.cpp /Fe:build\MapRanger.exe

if %ERRORLEVEL% equ 0 (
    echo Build successful.
) else (
    echo Build failed with error code %ERRORLEVEL%.
)
