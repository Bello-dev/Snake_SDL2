# Android Build Instructions for Snake SDL2

Complete Android implementation of Snake SDL2 game using SDL2's Android port with full touch controls, asset management, and native Android integration.

## 🚀 Quick Start

### Automated Build (Recommended)
```bash
# From project root directory
./build_android.sh
```

### Manual Build
```bash
cd android
./gradlew assembleDebug
```

## 📋 Requirements

1. **Android Studio** with NDK support (recommended) OR:
   - Android SDK (API level 21+)
   - Android NDK (version 23+)
   - Java 8+ (OpenJDK or Oracle JDK)

2. **SDL2 Source Code** with Android support
   - Download from: https://www.libsdl.org/download-2.0.php
   - Minimum version: SDL2 2.26.0

3. **Environment Variables**
   ```bash
   export ANDROID_HOME=/path/to/Android/Sdk
   export ANDROID_NDK_HOME=$ANDROID_HOME/ndk/[version]
   ```

## 🔧 Setup Instructions

### Step 1: Install Android Development Tools

**Option A: Android Studio (Recommended)**
1. Download and install Android Studio
2. Install NDK through SDK Manager:
   - Tools → SDK Manager → SDK Tools
   - Check "NDK (Side by side)" and "CMake"
   - Click "Apply"

**Option B: Command Line Tools**
1. Download Android command line tools
2. Install SDK and NDK:
   ```bash
   sdkmanager "platform-tools" "platforms;android-34" "ndk;25.2.9519653"
   ```

### Step 2: Download and Setup SDL2

1. **Download SDL2** source code:
   ```bash
   wget https://www.libsdl.org/release/SDL2-2.28.5.tar.gz
   tar -xzf SDL2-2.28.5.tar.gz
   ```

2. **Extract to Android project**:
   ```bash
   # From project root
   cp -r SDL2-2.28.5/* android/app/jni/SDL2/
   ```

3. **Verify SDL2 installation**:
   ```bash
   ls android/app/jni/SDL2/
   # Should contain: Android.mk, include/, src/, etc.
   ```

### Step 3: Build the Project

**Using automated script**:
```bash
./build_android.sh
```

**Using Gradle directly**:
```bash
cd android
./gradlew clean
./gradlew assembleDebug
```

**Using Android Studio**:
1. Open `android/` folder as project in Android Studio
2. Wait for Gradle sync to complete
3. Build → Make Project

## 📱 Installation and Testing

### Install on Device/Emulator
```bash
# Install debug APK
adb install android/app/build/outputs/apk/debug/app-debug.apk

# Install release APK (if built)
adb install android/app/build/outputs/apk/release/app-release.apk
```

### View Logs
```bash
adb logcat | grep -E "(SnakeSDL2|SDL|GAME)"
```

## 🎮 Android-Specific Features

### Touch Controls
- **Swipe gestures**: Control snake direction
  - Swipe Up: Move up
  - Swipe Down: Move down
  - Swipe Left: Move left
  - Swipe Right: Move right
- **Short tap**: Pause/resume game (equivalent to Space key)
- **Back button**: Exit game (equivalent to ESC key)

### Display Features
- **Landscape orientation**: Optimized for horizontal gameplay
- **Fullscreen immersive**: Hides navigation and status bars
- **Screen always on**: Prevents screen from dimming during gameplay
- **Hardware acceleration**: GPU-accelerated rendering

### Asset Management
- Fonts and textures bundled in APK
- Android-specific asset paths
- Optimized loading for mobile devices

## 🏗️ Project Structure

```
android/
├── app/
│   ├── build.gradle              # Module build configuration
│   ├── proguard-rules.pro        # Code obfuscation rules
│   ├── CMakeLists.txt            # CMake build configuration (alternative)
│   ├── src/main/
│   │   ├── AndroidManifest.xml   # App configuration and permissions
│   │   ├── assets/               # Game assets (fonts, textures)
│   │   ├── java/com/example/snake/
│   │   │   └── MainActivity.java # Main Activity with SDL2 integration
│   │   └── res/                  # Android resources (icons, etc.)
│   └── jni/
│       ├── Android.mk           # NDK build configuration
│       ├── Application.mk       # NDK application settings
│       ├── SDL2/                # SDL2 source code (you need to add this)
│       └── src/                 # Game source files
│           ├── main.c
│           ├── game.c
│           ├── game.h
│           ├── snake.c
│           ├── food.c
│           └── graphics.c
├── build.gradle                 # Root build configuration  
├── gradle.properties            # Gradle properties
├── settings.gradle              # Project settings
├── gradlew                      # Gradle wrapper (Unix)
├── gradlew.bat                  # Gradle wrapper (Windows)  
└── gradle/wrapper/
    ├── gradle-wrapper.jar       # Gradle wrapper JAR
    └── gradle-wrapper.properties # Wrapper configuration
```

## 🔍 Troubleshooting

### Common Issues

**SDL2 not found during build**:
```
Solution: Ensure SDL2 source is correctly extracted to android/app/jni/SDL2/
Check that Android.mk exists in SDL2 directory
```

**NDK build fails**:
```
Solution: Verify ANDROID_NDK_HOME is set correctly
Make sure NDK version is compatible (23+)
```

**Gradle sync fails**:
```
Solution: Check internet connection for dependency downloads
Ensure Android SDK and tools are properly installed
```

**APK crashes on startup**:
```
Solution: Check logcat for native crashes
Ensure all required libraries are included
Verify device supports OpenGL ES 2.0
```

### Performance Tips

- Use release build for final deployment: `./gradlew assembleRelease`
- Enable ProGuard for code optimization in release builds
- Test on various screen sizes and orientations
- Monitor memory usage with Android Profiler

### Debug Build vs Release Build

**Debug Build**:
- Includes debug symbols
- Larger APK size
- Better for development and testing
- Generated at: `app/build/outputs/apk/debug/`

**Release Build**:
- Optimized and minified
- Smaller APK size  
- Signed for distribution
- Generated at: `app/build/outputs/apk/release/`

## 📦 Distribution

### Signing for Play Store
1. Generate signing key
2. Configure signing in `build.gradle`
3. Build signed release APK
4. Upload to Google Play Console

### Alternative Distribution
- Direct APK installation (enable "Unknown sources")
- F-Droid (open source apps)
- Amazon Appstore
- Side-loading via ADB

## 🎯 Next Steps

- Add haptic feedback for enhanced mobile experience  
- Implement Android-specific achievements and leaderboards
- Add support for Android TV and larger screens
- Optimize battery usage with background processing controls

---

🐍 **Happy Gaming on Android!** 🐍