cmake_minimum_required(VERSION 3.10)
project(Snake_SDL2 VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Platform detection
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
elseif(ANDROID)
    set(PLATFORM_ANDROID TRUE)
endif()

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -DDEBUG")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -DNDEBUG")
endif()

# Find required packages
find_package(PkgConfig REQUIRED)

# SDL2 configuration
if(PLATFORM_WINDOWS)
    # Windows: Use vcpkg or manual SDL2 installation
    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED) 
    find_package(SDL2_mixer REQUIRED)
    
    set(SDL_LIBRARIES SDL2::SDL2 SDL2::SDL2main SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer)
elseif(PLATFORM_ANDROID)
    # Android: SDL2 will be handled by the Android build system
    find_library(SDL2_LIBRARY SDL2)
    find_library(SDL2_TTF_LIBRARY SDL2_ttf)
    find_library(SDL2_MIXER_LIBRARY SDL2_mixer)
    
    set(SDL_LIBRARIES ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARY} ${SDL2_MIXER_LIBRARY})
else()
    # Linux/Unix: Use pkg-config
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    
    set(SDL_LIBRARIES ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
    set(SDL_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
    set(SDL_CFLAGS ${SDL2_CFLAGS_OTHER} ${SDL2_TTF_CFLAGS_OTHER} ${SDL2_MIXER_CFLAGS_OTHER})
endif()

# Source files
set(SOURCES
    src/main.c
    src/game.c
    src/snake.c
    src/food.c
    src/graphics.c
)

# Create executable
add_executable(snake ${SOURCES})

# Include directories
target_include_directories(snake PRIVATE ${SDL_INCLUDE_DIRS})

# Link libraries
target_link_libraries(snake ${SDL_LIBRARIES} m)

# Compiler-specific flags
target_compile_options(snake PRIVATE ${SDL_CFLAGS})

# Platform-specific configurations
if(PLATFORM_WINDOWS)
    # Windows-specific settings
    set_target_properties(snake PROPERTIES WIN32_EXECUTABLE TRUE)
    
    # Copy SDL DLLs to output directory on Windows
    if(TARGET SDL2::SDL2)
        add_custom_command(TARGET snake POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:snake>)
    endif()
elseif(PLATFORM_ANDROID)
    # Android-specific settings
    set_target_properties(snake PROPERTIES
        ANDROID_APK TRUE
        ANDROID_APK_API_LEVEL 21)
endif()

# Install target
install(TARGETS snake RUNTIME DESTINATION bin)

# Add custom targets for convenience
add_custom_target(run
    COMMAND snake
    DEPENDS snake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Add clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt)