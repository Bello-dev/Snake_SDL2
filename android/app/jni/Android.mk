LOCAL_PATH := $(call my-dir)

# Include SDL3 main project
include $(LOCAL_PATH)/SDL3/Android.mk

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
    $(LOCAL_PATH)/SDL3/include \
    $(LOCAL_PATH)/SDL3/src

# Compiler flags
LOCAL_CFLAGS := -Wall -Wextra -std=c99 -O2 -DPLATFORM_ANDROID

# Link libraries
LOCAL_SHARED_LIBRARIES := SDL3 SDL3_mixer SDL3_ttf
LOCAL_LDLIBS := -llog -lGLESv1_CM -lGLESv2 -lm

# Export ANativeActivity_onCreate for SDL3
LOCAL_EXPORT_LDLIBS := -llog -lGLESv1_CM -lGLESv2

include $(BUILD_SHARED_LIBRARY)