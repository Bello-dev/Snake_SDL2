LOCAL_PATH := $(call my-dir)

# Include SDL2 main project
include $(LOCAL_PATH)/SDL2/Android.mk

# Main Snake game library
include $(CLEAR_VARS)
LOCAL_MODULE := snake

# Source files
LOCAL_SRC_FILES := \
    src/main.c \
    src/game.c \
    src/snake.c \
    src/food.c \
    src/graphics.c

# Include directories
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/SDL2/include \
    $(LOCAL_PATH)/SDL2/src

# Compiler flags
LOCAL_CFLAGS := -Wall -Wextra -std=c99 -O2 -DPLATFORM_ANDROID

# Link libraries
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer SDL2_ttf
LOCAL_LDLIBS := -llog -lGLESv1_CM -lGLESv2 -lm

# Export ANativeActivity_onCreate for SDL2
LOCAL_EXPORT_LDLIBS := -llog -lGLESv1_CM -lGLESv2

include $(BUILD_SHARED_LIBRARY)