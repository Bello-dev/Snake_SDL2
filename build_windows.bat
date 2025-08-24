@echo off
REM Windows build script for Snake SDL2
REM Requirements: Visual Studio Build Tools, CMake, vcpkg (for SDL2)

echo Building Snake SDL2 for Windows...

REM Check if CMake is available
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring project...
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
if %errorlevel% neq 0 (
    echo Error: CMake configuration failed
    echo Make sure SDL2, SDL2_ttf, and SDL2_mixer are installed via vcpkg:
    echo   vcpkg install sdl2 sdl2-ttf sdl2-mixer
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo Error: Build failed
    exit /b 1
)

echo Build completed successfully!
echo Executable location: build\Release\snake.exe
echo.
echo To run the game:
echo   cd build\Release
echo   snake.exe