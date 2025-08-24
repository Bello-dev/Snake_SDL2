#!/bin/bash
# Linux build script for Snake SDL3

echo "Building Snake SDL3 for Linux..."

# Function to detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo $ID
    elif [ -f /etc/debian_version ]; then
        echo "debian"
    elif [ -f /etc/fedora-release ]; then
        echo "fedora"
    elif [ -f /etc/arch-release ]; then
        echo "arch"
    else
        echo "unknown"
    fi
}

# Function to install dependencies automatically
install_dependencies() {
    local distro=$(detect_distro)
    echo "Detected distribution: $distro"
    
    case $distro in
        "ubuntu"|"debian"|"pop")
            echo "📦 Installing dependencies for Ubuntu/Debian..."
            if ! sudo apt update; then
                echo "❌ Failed to update package list. Please check your sudo permissions."
                exit 1
            fi
            if ! sudo apt install -y build-essential cmake libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev; then
                echo "❌ Failed to install dependencies. Please install manually:"
                echo "sudo apt install build-essential cmake libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev"
                exit 1
            fi
            ;;
        "fedora"|"rhel"|"centos")
            echo "📦 Installing dependencies for Fedora/RHEL/CentOS..."
            # Try SDL3 first, fallback to SDL2
            if ! sudo dnf install -y gcc cmake SDL3-devel SDL3_ttf-devel SDL3_mixer-devel 2>/dev/null; then
                echo "SDL3 not available, installing SDL2 as fallback..."
                if ! sudo dnf install -y gcc cmake SDL2-devel SDL2_ttf-devel SDL2_mixer-devel; then
                    echo "❌ Failed to install dependencies. Please install manually:"
                    echo "sudo dnf install gcc cmake SDL2-devel SDL2_ttf-devel SDL2_mixer-devel"
                    exit 1
                fi
            fi
            ;;
        "arch"|"manjaro")
            echo "📦 Installing dependencies for Arch Linux..."
            if ! sudo pacman -S --needed --noconfirm gcc cmake sdl2 sdl2_ttf sdl2_mixer; then
                echo "❌ Failed to install dependencies. Please install manually:"
                echo "sudo pacman -S gcc cmake sdl2 sdl2_ttf sdl2_mixer"
                exit 1
            fi
            ;;
        *)
            echo "⚠️  Unknown distribution. Please install dependencies manually:"
            echo "- build-essential or equivalent (gcc, make)"
            echo "- cmake"
            echo "- SDL2 development libraries (libsdl2-dev, libsdl2-ttf-dev, libsdl2-mixer-dev)"
            read -p "Continue anyway? (y/N): " -n 1 -r
            echo
            if [[ ! $REPLY =~ ^[Yy]$ ]]; then
                exit 1
            fi
            ;;
    esac
}

# Check if we should install dependencies automatically
if ! command -v cmake &> /dev/null || ! pkg-config --exists sdl2 2>/dev/null; then
    echo "🔍 Missing dependencies detected..."
    read -p "Install dependencies automatically? (Y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Nn]$ ]]; then
        install_dependencies
    else
        echo "❌ Dependencies required but installation skipped."
        if ! command -v cmake &> /dev/null; then
            echo "Error: CMake is not installed"
            echo "Install with: sudo apt install cmake (Ubuntu/Debian)"
        fi
        if ! pkg-config --exists sdl2; then
            echo "Error: SDL2 development libraries not found"
            echo "Install with appropriate command for your distribution"
        fi
        exit 1
    fi
else
    echo "✅ Dependencies already installed"
fi

# Verify SDL libraries are now available
if ! pkg-config --exists sdl3; then
    echo "Warning: SDL3 development libraries not found, trying SDL2 as fallback"
    if ! pkg-config --exists sdl2; then
        echo "Error: SDL libraries still not found after installation attempt"
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