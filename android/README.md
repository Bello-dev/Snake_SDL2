# Android Build Instructions for Snake SDL2

This directory contains the necessary files to build Snake SDL2 for Android using SDL2's Android port.

## Requirements

1. Android SDK and NDK
2. CMake (included with Android Studio)
3. SDL2 source code with Android support

## Setup Instructions

1. Install Android Studio with NDK support
2. Download SDL2 source with Android support from libsdl.org
3. Extract SDL2 to `android/app/jni/SDL2/`
4. Copy the game source files to `android/app/jni/src/`

## Build Process

1. Open the `android/` project in Android Studio
2. Build and run the project
3. Or use command line:
   ```bash
   cd android
   ./gradlew assembleDebug
   ```

## File Structure
```
android/
├── app/
│   ├── src/main/
│   │   ├── java/com/example/snake/
│   │   │   └── MainActivity.java
│   │   └── AndroidManifest.xml
│   ├── jni/
│   │   ├── SDL2/          # SDL2 source code
│   │   ├── src/           # Game source files (symbolic links)
│   │   └── Android.mk     # NDK build configuration
│   └── build.gradle
└── build.gradle
```

## Note
This is a basic structure. Full Android implementation requires additional 
configuration for input handling, asset management, and proper Java bindings.