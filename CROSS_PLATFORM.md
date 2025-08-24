# Cross-Platform Build Guide for Snake SDL3

Snake SDL3 now supports building and running on multiple platforms: **Windows**, **Linux**, and **Android**.

## 🚀 Quick Start

### Linux (All Distributions)
```bash
# Simple one-command build with automatic dependency installation
./build_linux.sh

# Run the game
cd build && ./snake
```

The build script now automatically detects your Linux distribution and installs the required dependencies:
- **Ubuntu/Debian/Pop!_OS**: `build-essential cmake libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev`
- **Fedora/RHEL/CentOS**: `gcc cmake SDL2-devel SDL2_ttf-devel SDL2_mixer-devel`
- **Arch/Manjaro**: `gcc cmake sdl2 sdl2_ttf sdl2_mixer`

### Windows (Visual Studio)
```cmd
REM Simple one-command build with automatic dependency installation
build_windows.bat

REM Run the game
cd build\Release
snake.exe
```

The build script automatically installs dependencies via vcpkg or Chocolatey when available.

### Android
```bash
# Simple one-command build with automatic setup
./build_android.sh

# Install on device (APK will be built automatically)
adb install android/app/build/outputs/apk/debug/app-debug.apk
```

The build script automatically:
- Installs Android SDK/NDK if not present
- Downloads SDL3 source code
- Sets up the build environment

See `android/README.md` for detailed Android build instructions.

## 🛠️ Manual Build (CMake)

If you prefer to manage dependencies manually or the automatic installation doesn't work:

### Prerequisites
- CMake 3.10 or higher
- C compiler (GCC, Clang, MSVC)
- SDL3, SDL3_ttf, SDL3_mixer development libraries (or SDL2 as fallback)

### Build Steps
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Platform-Specific CMake Options

#### Windows with vcpkg:
```cmd
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
```

#### Android:
```bash  
cmake .. -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21
```

## 🔄 Automatic Dependency Installation

All build scripts now include automatic dependency installation for a seamless build experience:

### Linux Auto-Installation
- **Distribution Detection**: Automatically detects Ubuntu, Debian, Fedora, Arch, and more
- **Package Manager Integration**: Uses `apt`, `dnf`, or `pacman` as appropriate
- **User Control**: Prompts before installation, allows manual override
- **Error Handling**: Graceful fallback with clear error messages

### Windows Auto-Installation  
- **vcpkg Integration**: Automatically installs SDL2 packages via vcpkg when available
- **Chocolatey Fallback**: Uses Chocolatey for CMake installation if vcpkg not found
- **Environment Detection**: Automatically sets up VCPKG_ROOT when possible

### Android Auto-Installation
- **SDK/NDK Setup**: Downloads and configures Android SDK and NDK automatically
- **SDL3 Source**: Automatically downloads SDL3 source from GitHub
- **Environment Configuration**: Sets up all required environment variables

### Manual Override
If you prefer manual dependency management, you can:
- Skip automatic installation by answering "n" to prompts
- Set dependencies manually and run build scripts normally
- Use the manual build process with CMake directly

## 🎮 Cross-Platform Features

### Automatic Font Detection
The game automatically detects and loads system fonts:
- **Windows**: Uses Arial from Windows/Fonts/
- **Linux**: Searches Liberation Sans, DejaVu Sans in common locations
- **Android**: Uses Roboto system font
- **Graceful fallback**: Game works without fonts (graphics only)

### Platform Detection
Runtime platform detection enables platform-specific optimizations:
- Windows: DirectSound audio backend preference
- Linux: ALSA/PulseAudio audio backend  
- Android: OpenSL ES audio backend
- Proper file path handling per platform

### Input Handling
- **Desktop**: Keyboard controls (Arrow keys, WASD, Space, Escape)
- **Android**: Touch controls with swipe gestures and tap support
  - Swipe gestures for directional movement
  - Tap to pause/resume game
  - Back button for exit

## 📁 Project Structure
```
Snake_SDL3/
├── src/              # Cross-platform game source
├── build_linux.sh    # Linux build script
├── build_windows.bat # Windows build script  
├── build_android.sh   # Android build script
├── android/          # Complete Android project implementation
├── CMakeLists.txt    # CMake build configuration
├── Makefile          # Legacy Linux Makefile (still works)
└── CROSS_PLATFORM.md # This documentation
```

## 🔧 Troubleshooting

### General Issues
- **Automatic installation failed**: Run build script with manual installation mode (`n` when prompted)
- **Permission denied**: Ensure build scripts are executable (`chmod +x build_*.sh`)
- **Network issues**: Automatic downloads require internet connection for initial setup

### Linux Issues
- **Distribution not detected**: Script will prompt for manual dependency installation
- **Package manager locked**: Wait for other package operations to complete, then retry
- **Sudo required**: Automatic installation requires sudo privileges for package management
- **SDL3 not found**: Automatically falls back to SDL2, which is installed by the script
- **Audio warnings**: Normal in headless/VM environments

### Windows Issues  
- **vcpkg not found**: Install vcpkg from GitHub or use manual dependency installation
- **MSVC not found**: Install Visual Studio Build Tools or Visual Studio Community
- **VCPKG_ROOT not set**: Set environment variable or let script try automatic detection
- **DLL missing**: vcpkg automatically handles DLL placement

### Android Issues
- **SDK download failed**: Check internet connection or install Android Studio manually
- **NDK installation failed**: Try installing via Android Studio SDK Manager
- **SDL3 download failed**: Check network connection or download SDL3 source manually
- **Gradle errors**: Ensure JAVA_HOME is set and Java 8+ is installed
- **APK crashes**: Check logcat for native library loading issues

## 🎯 Platform-Specific Notes

### Windows
- Supports Windows 7 and later
- Uses system fonts (Arial fallback)
- Hardware-accelerated rendering via DirectX/OpenGL
- Executable requires SDL3 DLLs

### Linux
- Supports most modern distributions
- Auto-detects Liberation Sans or DejaVu fonts
- Uses native audio backends (ALSA/PulseAudio)
- Static linking option available

### Android  
- Minimum API level 21 (Android 5.0+)
- ARM64, ARM32, and x86/x86_64 architectures supported  
- Complete touch input implementation with swipe gestures
- Landscape orientation optimized for gameplay
- Immersive fullscreen with system UI hiding
- Proper asset management and font loading
- APK ready for Play Store distribution

## 🚀 Future Enhancements
- [ ] iOS support via SDL3
- [ ] macOS native build  
- [ ] WebAssembly (Emscripten) support
- [x] Touch controls for mobile platforms ✅
- [ ] Platform-specific UI scaling
- [ ] Distribution packages (AppImage, MSI, APK)
- [ ] Haptic feedback for Android
- [ ] Android TV support