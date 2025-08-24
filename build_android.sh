#!/bin/bash
# Android build script for Snake SDL3
# Requirements: Android SDK, NDK, SDL3 source code

set -e

echo "🐍 Building Snake SDL3 for Android 🐍"
echo "===================================="
echo

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR/.."
ANDROID_DIR="$PROJECT_ROOT/android"

# Function to install Android SDK via command line tools
install_android_sdk() {
    echo "📦 Installing Android SDK..."
    local sdk_dir="$HOME/Android/Sdk"
    
    # Create directory
    mkdir -p "$sdk_dir"
    cd "$sdk_dir"
    
    # Download command line tools
    echo "Downloading Android command line tools..."
    if command -v curl &> /dev/null; then
        curl -o commandlinetools.zip "https://dl.google.com/android/repository/commandlinetools-linux-9477386_latest.zip"
    elif command -v wget &> /dev/null; then
        wget -O commandlinetools.zip "https://dl.google.com/android/repository/commandlinetools-linux-9477386_latest.zip"
    else
        echo "❌ Neither curl nor wget found. Please install one of them first."
        exit 1
    fi
    
    # Extract tools
    unzip -q commandlinetools.zip
    mkdir -p cmdline-tools
    mv cmdline-tools/* cmdline-tools/latest/
    rm commandlinetools.zip
    
    # Set environment
    export ANDROID_HOME="$sdk_dir"
    export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$PATH"
    
    # Install platform tools and NDK
    echo "Installing platform tools and NDK..."
    yes | sdkmanager --licenses
    sdkmanager "platform-tools" "platforms;android-29" "build-tools;30.0.3" "ndk;21.4.7075529"
    
    export ANDROID_NDK_HOME="$ANDROID_HOME/ndk/21.4.7075529"
    
    echo "✅ Android SDK/NDK installed successfully"
    echo "Please add the following to your ~/.bashrc or ~/.profile:"
    echo "export ANDROID_HOME=\"$ANDROID_HOME\""
    echo "export ANDROID_NDK_HOME=\"$ANDROID_NDK_HOME\""
    echo "export PATH=\"\$ANDROID_HOME/platform-tools:\$PATH\""
}

# Function to download SDL3 source
download_sdl3_source() {
    echo "📦 Downloading SDL3 source..."
    local sdl3_url="https://github.com/libsdl-org/SDL/archive/refs/heads/main.zip"
    local temp_dir="/tmp/sdl3_download"
    
    mkdir -p "$temp_dir"
    cd "$temp_dir"
    
    if command -v curl &> /dev/null; then
        curl -L -o sdl3-main.zip "$sdl3_url"
    elif command -v wget &> /dev/null; then
        wget -O sdl3-main.zip "$sdl3_url"
    else
        echo "❌ Neither curl nor wget found. Please install one of them first."
        exit 1
    fi
    
    unzip -q sdl3-main.zip
    
    # Create target directory and copy source
    mkdir -p "$ANDROID_DIR/app/jni/SDL3"
    cp -r SDL-main/* "$ANDROID_DIR/app/jni/SDL3/"
    
    # Clean up
    cd "$SCRIPT_DIR"
    rm -rf "$temp_dir"
    
    echo "✅ SDL3 source downloaded successfully"
}

# Check requirements and offer automatic installation
echo "📋 Checking requirements..."

# Check if Android SDK is available
if [ -z "$ANDROID_HOME" ] || [ ! -d "$ANDROID_HOME" ]; then
    echo "❌ ANDROID_HOME is not set or directory doesn't exist"
    read -p "Install Android SDK automatically? (Y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Nn]$ ]]; then
        install_android_sdk
    else
        echo "Please install Android SDK manually and set ANDROID_HOME environment variable"
        echo "Example: export ANDROID_HOME=/path/to/Android/Sdk"
        exit 1
    fi
else
    echo "✅ ANDROID_HOME: $ANDROID_HOME"
fi

# Check if Android NDK is available  
if [ -z "$ANDROID_NDK_HOME" ]; then
    if [ -d "$ANDROID_HOME/ndk-bundle" ]; then
        export ANDROID_NDK_HOME="$ANDROID_HOME/ndk-bundle"
    elif [ -d "$ANDROID_HOME/ndk" ]; then
        # Find latest NDK version
        LATEST_NDK=$(ls "$ANDROID_HOME/ndk" | sort -V | tail -1)
        export ANDROID_NDK_HOME="$ANDROID_HOME/ndk/$LATEST_NDK"
    else
        echo "❌ Android NDK not found"
        if [ -d "$ANDROID_HOME" ]; then
            echo "Installing NDK via SDK Manager..."
            "$ANDROID_HOME/cmdline-tools/latest/bin/sdkmanager" "ndk;21.4.7075529" || {
                echo "Failed to install NDK automatically"
                echo "Please install Android NDK through Android Studio or manually"
                exit 1
            }
            export ANDROID_NDK_HOME="$ANDROID_HOME/ndk/21.4.7075529"
        else
            echo "Please install Android NDK through Android Studio or manually"
            exit 1
        fi
    fi
fi

echo "✅ ANDROID_NDK_HOME: $ANDROID_NDK_HOME"

# Check for SDL3 source
SDL3_PATH="$ANDROID_DIR/app/jni/SDL3"
if [ ! -d "$SDL3_PATH" ] || [ ! -f "$SDL3_PATH/include/SDL3/SDL.h" ]; then
    echo "❌ SDL3 source not found at $SDL3_PATH"
    read -p "Download SDL3 source automatically? (Y/n): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Nn]$ ]]; then
        download_sdl3_source
    else
        echo ""
        echo "To fix this manually:"
        echo "1. Download SDL3 source with Android support from https://www.libsdl.org/download-3.0.php"
        echo "2. Extract to $SDL3_PATH"
        echo "3. Ensure the following files exist:"
        echo "   - $SDL3_PATH/Android.mk"
        echo "   - $SDL3_PATH/include/SDL3/SDL.h"
        echo ""
        exit 1
    fi
fi

echo "✅ SDL3 source found"

# Check for gradlew
if [ ! -f "$ANDROID_DIR/gradlew" ]; then
    echo "❌ gradlew not found"
    echo "Creating gradle wrapper..."
    cd "$ANDROID_DIR"
    if command -v gradle &> /dev/null; then
        gradle wrapper
    else
        echo "❌ Gradle not found. Please install Gradle first:"
        echo "Ubuntu/Debian: sudo apt install gradle"
        echo "Or download from: https://gradle.org/install/"
        exit 1
    fi
fi

echo "✅ Gradle wrapper ready"

# Make gradlew executable
chmod +x "$ANDROID_DIR/gradlew"

echo ""
echo "🔨 Building Android APK..."
echo ""

# Build the project
cd "$ANDROID_DIR"

# Clean previous build
echo "🧹 Cleaning previous build..."
./gradlew clean

# Build debug APK
echo "🔧 Building debug APK..."
./gradlew assembleDebug

# Check if build succeeded
if [ $? -eq 0 ]; then
    APK_PATH="$ANDROID_DIR/app/build/outputs/apk/debug/app-debug.apk"
    if [ -f "$APK_PATH" ]; then
        echo ""
        echo "🎉 Build completed successfully!"
        echo "📱 APK location: $APK_PATH"
        echo ""
        echo "📋 To install on device:"
        echo "   adb install '$APK_PATH'"
        echo ""
        echo "🔧 To build release APK:"
        echo "   cd android && ./gradlew assembleRelease"
        echo ""
    else
        echo "❌ Build succeeded but APK not found"
        exit 1
    fi
else
    echo "❌ Build failed"
    exit 1
fi