cmake_minimum_required(VERSION 3.10.2)
project(SnakeSDL2Android C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find SDL2 for Android
find_library(SDL2_LIBRARY SDL2)
find_library(SDL2_TTF_LIBRARY SDL2_ttf) 
find_library(SDL2_MIXER_LIBRARY SDL2_mixer)
find_library(LOG_LIB log)
find_library(GLES2_LIB GLESv2)
find_library(GLES1_LIB GLESv1_CM)

# Source files
set(SOURCES
    src/main.c
    src/game.c
    src/snake.c
    src/food.c
    src/graphics.c
)

# Create shared library for Android
add_library(snake SHARED ${SOURCES})

# Include directories  
target_include_directories(snake PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/SDL2/include
)

# Compile definitions
target_compile_definitions(snake PRIVATE
    PLATFORM_ANDROID
)

# Compiler flags
target_compile_options(snake PRIVATE
    -Wall
    -Wextra
    -O2
)

# Link libraries
target_link_libraries(snake
    ${SDL2_LIBRARY}
    ${SDL2_TTF_LIBRARY}
    ${SDL2_MIXER_LIBRARY}
    ${LOG_LIB}
    ${GLES2_LIB}
    ${GLES1_LIB}
    m
)