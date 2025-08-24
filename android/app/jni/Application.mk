# Application.mk for Snake SDL2
# This file defines project-wide NDK build settings

# Target Android API level
APP_PLATFORM := android-21

# Target ABIs (Application Binary Interfaces)
# arm64-v8a: 64-bit ARM
# armeabi-v7a: 32-bit ARM
# x86: 32-bit Intel
# x86_64: 64-bit Intel
APP_ABI := arm64-v8a armeabi-v7a x86 x86_64

# C++ runtime
APP_STL := c++_static

# Build mode (release or debug)
APP_OPTIM := release

# Enable C++14 features
APP_CPPFLAGS := -std=c++14

# Additional flags
APP_CFLAGS := -Wall -Wextra

# Modules to build
APP_MODULES := snake

# Allow undefined symbols (helpful for debugging)
APP_ALLOW_MISSING_DEPS := true