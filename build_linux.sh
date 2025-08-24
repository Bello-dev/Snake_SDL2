#!/bin/bash
# Linux build script for Snake SDL2

echo "Building Snake SDL2 for Linux..."

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed"
    echo "Install with: sudo apt install cmake"
    exit 1
fi

# Check if SDL2 development libraries are installed
if ! pkg-config --exists sdl2; then
    echo "Error: SDL2 development libraries not found"
    echo "Install with:"
    echo "  Ubuntu/Debian: sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev"
    echo "  Fedora: sudo dnf install SDL2-devel SDL2_ttf-devel SDL2_mixer-devel"
    echo "  Arch: sudo pacman -S sdl2 sdl2_ttf sdl2_mixer"
    exit 1
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