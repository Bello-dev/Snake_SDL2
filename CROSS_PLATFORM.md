# Cross-Platform Build Guide for Snake SDL2

Snake SDL2 now supports building and running on multiple platforms: **Windows**, **Linux**, and **Android**.

## 🚀 Quick Start

### Linux (Ubuntu/Debian/Pop!_OS)
```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev

# Build and run
./build_linux.sh
cd build && ./snake
```

### Linux (Fedora/RHEL/CentOS)
```bash
# Install dependencies
sudo dnf install gcc cmake SDL2-devel SDL2_ttf-devel SDL2_mixer-devel

# Build and run
./build_linux.sh
cd build && ./snake
```

### Linux (Arch Linux/Manjaro)
```bash
# Install dependencies
sudo pacman -S gcc cmake sdl2 sdl2_ttf sdl2_mixer

# Build and run  
./build_linux.sh
cd build && ./snake
```

### Windows (Visual Studio)
```cmd
REM Install dependencies via vcpkg
vcpkg install sdl2 sdl2-ttf sdl2-mixer

REM Build and run
build_windows.bat
cd build\Release
snake.exe
```

### Android
See `android/README.md` for detailed Android build instructions.

## 🛠️ Manual Build (CMake)

### Prerequisites
- CMake 3.10 or higher
- C compiler (GCC, Clang, MSVC)
- SDL2, SDL2_ttf, SDL2_mixer development libraries

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
- **Desktop**: Keyboard controls (Arrow keys, WASP, Space, Escape)
- **Android**: Touch controls with virtual D-pad (future enhancement)

## 📁 Project Structure
```
Snake_SDL2/
├── src/              # Cross-platform game source
├── build_linux.sh    # Linux build script
├── build_windows.bat # Windows build script  
├── android/          # Android project files
├── CMakeLists.txt    # CMake build configuration
├── Makefile          # Legacy Linux Makefile (still works)
└── CROSS_PLATFORM.md # This documentation
```

## 🔧 Troubleshooting

### Linux Issues
- **SDL2 not found**: Install dev packages (`libsdl2-dev`)
- **Permission denied**: Make sure `build_linux.sh` is executable
- **Audio warnings**: Normal in headless/VM environments

### Windows Issues  
- **SDL2 not found**: Install via vcpkg or download SDL2 dev libraries
- **MSVC not found**: Install Visual Studio Build Tools
- **DLL missing**: Ensure SDL2 DLLs are in PATH or same directory

### Android Issues
- **NDK not found**: Install Android NDK via SDK Manager
- **SDL2 missing**: Download SDL2 source with Android support
- **Gradle errors**: Check Android SDK/NDK versions

## 🎯 Platform-Specific Notes

### Windows
- Supports Windows 7 and later
- Uses system fonts (Arial fallback)
- Hardware-accelerated rendering via DirectX/OpenGL
- Executable requires SDL2 DLLs

### Linux
- Supports most modern distributions
- Auto-detects Liberation Sans or DejaVu fonts
- Uses native audio backends (ALSA/PulseAudio)
- Static linking option available

### Android  
- Minimum API level 21 (Android 5.0)
- ARM and x86 architectures supported
- Touch input adaptation needed
- APK signing required for distribution

## 🚀 Future Enhancements
- [ ] iOS support via SDL2
- [ ] macOS native build
- [ ] WebAssembly (Emscripten) support
- [ ] Touch controls for mobile platforms
- [ ] Platform-specific UI scaling
- [ ] Distribution packages (AppImage, MSI, APK)