LOCAL_PATH := $(call my-dir)

# SDL2 library
include $(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := SDL2/lib/$(TARGET_ARCH_ABI)/libSDL2.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_ttf
LOCAL_SRC_FILES := SDL2/lib/$(TARGET_ARCH_ABI)/libSDL2_ttf.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := SDL2_mixer
LOCAL_SRC_FILES := SDL2/lib/$(TARGET_ARCH_ABI)/libSDL2_mixer.so
include $(PREBUILT_SHARED_LIBRARY)

# Main Snake game library
include $(CLEAR_VARS)
LOCAL_MODULE := snake
LOCAL_C_INCLUDES := $(LOCAL_PATH)/SDL2/include
LOCAL_SRC_FILES := \
    src/main.c \
    src/game.c \
    src/snake.c \
    src/food.c \
    src/graphics.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer
LOCAL_CFLAGS := -Wall -Wextra -std=c99 -O2 -DPLATFORM_ANDROID
LOCAL_LDLIBS := -llog -lGLESv1_CM -lGLESv2

include $(BUILD_SHARED_LIBRARY)