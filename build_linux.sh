#!/bin/bash
# Linux build script for Snake SDL3

echo "Building Snake SDL3 for Linux..."

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed"
    echo "Install with: sudo apt install cmake"
    exit 1
fi

# Check if SDL3 development libraries are installed, fallback to SDL2
if ! pkg-config --exists sdl3; then
    echo "Warning: SDL3 development libraries not found, trying SDL2 as fallback"
    if ! pkg-config --exists sdl2; then
        echo "Error: Neither SDL3 nor SDL2 development libraries found"
        echo "Install with:"
        echo "  For SDL3 (preferred): sudo apt install libsdl3-dev libsdl3-ttf-dev libsdl3-mixer-dev"
        echo "  For SDL2 (fallback): sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev"
        echo "  Fedora: sudo dnf install SDL3-devel SDL3_ttf-devel SDL3_mixer-devel"
        echo "  Arch: sudo pacman -S sdl3 sdl3_ttf sdl3_mixer"
        exit 1
    else
        echo "✅ Using SDL2 as fallback (SDL3 preferred but not available)"
    fi
else
    echo "✅ SDL3 development libraries found"
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed"
    exit 1
fi

# Build the project
echo "Building project..."
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    exit 1
fi

echo "Build completed successfully!"
echo "Executable location: build/snake"
echo ""
echo "To run the game:"
echo "  cd build"
echo "  ./snake"