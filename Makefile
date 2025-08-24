# Makefile for Snake SDL2

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lSDL2 -lSDL2_ttf -lSDL2_mixer -lm

# Directories
SRCDIR = src
BUILDDIR = build
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET = snake

# Default target
all: $(TARGET)

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Compile object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $(TARGET)

# Clean build files
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt update
	sudo apt install gcc libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev

# Install dependencies (Fedora)
install-deps-fedora:
	sudo dnf install gcc SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel

# Install dependencies (Arch Linux)
install-deps-arch:
	sudo pacman -S gcc sdl2 sdl2_image sdl2_ttf sdl2_mixer

# Run the game
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CFLAGS += -DNDEBUG
release: $(TARGET)

.PHONY: all clean install-deps install-deps-fedora install-deps-arch run debug release