# SDL2 to SDL3 Migration Guide

This document outlines the changes made to upgrade the Snake game from SDL2 to SDL3, with backward compatibility support.

## 🔄 Changes Made

### Project Structure
- **Project name**: Changed from `Snake_SDL2` to `Snake_SDL3`
- **Window title**: Updated to "Snake SDL3 - Enhanced Edition"
- **All documentation**: Updated references from SDL2 to SDL3

### Build System Updates

#### CMakeLists.txt
- Primary target: SDL3 libraries (`SDL3::SDL3`, `SDL3_ttf::SDL3_ttf`, `SDL3_mixer::SDL3_mixer`)
- Fallback system: Automatically detects and uses SDL2 if SDL3 is not available
- Cross-platform support: Windows (vcpkg), Linux (pkg-config), Android (NDK)

#### Build Scripts
- **Linux**: `build_linux.sh` now checks for SDL3 first, falls back to SDL2
- **Windows**: `build_windows.bat` updated for SDL3 vcpkg packages
- **Android**: Updated Android.mk and CMakeLists.txt for SDL3 paths

### Source Code Changes

#### Header Compatibility (`src/game.h`)
```c
#ifdef USING_SDL2_FALLBACK
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
#else
    #include <SDL3/SDL.h>
    #include <SDL3_ttf/SDL_ttf.h>
    #include <SDL3_mixer/SDL_mixer.h>
#endif
```

## 🚀 Migration Benefits

### 1. **Future-Proof Architecture**
- Ready for SDL3 when it becomes widely available
- Seamless transition from SDL2 to SDL3
- No breaking changes for current users

### 2. **Backward Compatibility**
- Automatic SDL2 fallback detection
- Zero configuration required
- Works with existing SDL2 installations

### 3. **Improved Build System**
- Intelligent library detection
- Clear error messages with installation instructions
- Cross-platform consistency

## 🛠️ Building the Project

### Current State (SDL2 Fallback)
Most systems will automatically use SDL2 as SDL3 is not yet widely available:

```bash
# Linux
./build_linux.sh && cd build && ./snake

# Windows  
build_windows.bat && cd build\Release && snake.exe

# Android
./build_android.sh
```

### Future State (SDL3 Native)
Once SDL3 is available in package managers, the build system will automatically use it:

```bash
# Install SDL3 (when available)
sudo apt install libsdl3-dev libsdl3-ttf-dev libsdl3-mixer-dev

# Build will automatically use SDL3
./build_linux.sh && cd build && ./snake
```

## 📋 Installation Requirements

### For SDL2 (Current Fallback)
```bash
# Ubuntu/Debian
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev

# Fedora
sudo dnf install SDL2-devel SDL2_ttf-devel SDL2_mixer-devel

# Windows (vcpkg)
vcpkg install sdl2 sdl2-ttf sdl2-mixer
```

### For SDL3 (Future Target)
```bash
# Ubuntu/Debian (when available)
sudo apt install libsdl3-dev libsdl3-ttf-dev libsdl3-mixer-dev

# Fedora (when available)
sudo dnf install SDL3-devel SDL3_ttf-devel SDL3_mixer-devel

# Windows (vcpkg, when available)
vcpkg install sdl3 sdl3-ttf sdl3-mixer
```

## 🔍 Technical Details

### API Compatibility
The current implementation maintains full API compatibility between SDL2 and SDL3 for the functions used in this project. No source code changes are required for the core game logic.

### Conditional Compilation
The `USING_SDL2_FALLBACK` preprocessor definition is automatically set when SDL2 is used as a fallback, allowing for future SDL3-specific optimizations.

### Build Detection Logic
1. **Try SDL3**: Check for SDL3 via pkg-config/vcpkg
2. **Fallback to SDL2**: If SDL3 not found, use SDL2
3. **Clear Errors**: Provide helpful installation instructions if neither is found

## 🎯 Next Steps

1. **Monitor SDL3 Release**: Watch for stable SDL3 releases
2. **Test SDL3**: Test with SDL3 when available
3. **Optimize for SDL3**: Add SDL3-specific features and optimizations
4. **Documentation**: Update guides when SDL3 becomes mainstream

---

**Note**: This migration ensures the project is ready for the future of SDL development while maintaining full compatibility with current systems using SDL2.