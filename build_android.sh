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

# Check requirements
echo "📋 Checking requirements..."

# Check if Android SDK is available
if [ -z "$ANDROID_HOME" ]; then
    echo "❌ ANDROID_HOME is not set"
    echo "Please install Android SDK and set ANDROID_HOME environment variable"
    echo "Example: export ANDROID_HOME=/path/to/Android/Sdk"
    exit 1
fi

echo "✅ ANDROID_HOME: $ANDROID_HOME"

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
        echo "Please install Android NDK through Android Studio or manually"
        exit 1
    fi
fi

echo "✅ ANDROID_NDK_HOME: $ANDROID_NDK_HOME"

# Check for SDL3 source
SDL3_PATH="$ANDROID_DIR/app/jni/SDL3"
if [ ! -d "$SDL3_PATH" ]; then
    echo "❌ SDL3 source not found at $SDL3_PATH"
    echo ""
    echo "To fix this:"
    echo "1. Download SDL3 source with Android support from https://www.libsdl.org/download-3.0.php"
    echo "2. Extract to $SDL3_PATH"
    echo "3. Ensure the following files exist:"
    echo "   - $SDL3_PATH/Android.mk"
    echo "   - $SDL3_PATH/include/SDL.h"
    echo ""
    exit 1
fi

echo "✅ SDL3 source found"

# Check for gradlew
if [ ! -f "$ANDROID_DIR/gradlew" ]; then
    echo "❌ gradlew not found"
    echo "Creating gradle wrapper..."
    cd "$ANDROID_DIR"
    gradle wrapper
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