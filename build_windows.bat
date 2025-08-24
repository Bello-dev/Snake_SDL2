@echo off
REM Windows build script for Snake SDL3
REM Requirements: Visual Studio Build Tools, CMake, vcpkg (for SDL3)

echo Building Snake SDL3 for Windows...

REM Function to install dependencies automatically
:install_dependencies
echo 📦 Attempting to install dependencies...

REM Check if vcpkg is available
where vcpkg >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ vcpkg found, installing SDL dependencies...
    vcpkg install sdl2 sdl2-ttf sdl2-mixer
    if %errorlevel% neq 0 (
        echo ❌ Failed to install dependencies via vcpkg
        echo Please install manually: vcpkg install sdl2 sdl2-ttf sdl2-mixer
        exit /b 1
    )
    echo ✅ Dependencies installed successfully via vcpkg
    goto :check_cmake
) else (
    echo ⚠️ vcpkg not found in PATH
    
    REM Check if chocolatey is available as fallback
    where choco >nul 2>&1
    if %errorlevel% equ 0 (
        echo 📦 Using Chocolatey to install CMake...
        choco install cmake --yes
        echo ✅ Please install SDL2 dependencies manually via vcpkg:
        echo   1. Install vcpkg: https://github.com/Microsoft/vcpkg
        echo   2. Run: vcpkg install sdl2 sdl2-ttf sdl2-mixer
        pause
    ) else (
        echo ❌ Neither vcpkg nor Chocolatey found
        echo Please install dependencies manually:
        echo   1. Install CMake from https://cmake.org/download/
        echo   2. Install vcpkg: https://github.com/Microsoft/vcpkg
        echo   3. Run: vcpkg install sdl2 sdl2-ttf sdl2-mixer
        exit /b 1
    )
)
goto :eof

:check_cmake
REM Check if CMake is available
cmake --version >nul 2>&1
if %errorlevel% equ 0 (
    echo ✅ CMake found
    goto :build
)

echo 🔍 CMake not found, attempting installation...
set /p install_deps=Install dependencies automatically? (Y/n): 
if /i "%install_deps%"=="n" (
    echo ❌ Dependencies required but installation skipped.
    echo Error: CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    exit /b 1
)

call :install_dependencies

:build

REM Create build directory
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo Configuring project...
if defined VCPKG_ROOT (
    cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
) else (
    echo ⚠️ VCPKG_ROOT not set, trying default CMake configuration...
    cmake ..
)

if %errorlevel% neq 0 (
    echo Error: CMake configuration failed
    if defined VCPKG_ROOT (
        echo Make sure SDL2, SDL2_ttf, and SDL2_mixer are installed via vcpkg:
        echo   vcpkg install sdl2 sdl2-ttf sdl2-mixer
    ) else (
        echo Please set VCPKG_ROOT environment variable and install SDL dependencies:
        echo   1. Install vcpkg: https://github.com/Microsoft/vcpkg
        echo   2. Set VCPKG_ROOT to vcpkg installation directory
        echo   3. Run: vcpkg install sdl2 sdl2-ttf sdl2-mixer
    )
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