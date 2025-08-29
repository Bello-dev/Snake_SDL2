# Snake SDL2 - Revolutionary Edition Makefile

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2_gfx -lm

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = data

# Source and object files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/snake_revolutionary

# Default target
.PHONY: all
all: $(TARGET)

# Create directories
$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LIBS)
	@echo "✅ Revolutionary Snake Game compiled successfully!"

# Run the game
.PHONY: run
run: $(TARGET)
	@clear
	@echo "🐍 Starting Snake SDL2 - Revolutionary Edition..."
	cd $(BINDIR) && ./snake_revolutionary

# Clean build files
.PHONY: clean
clean:
	@rm -rf $(OBJDIR)
	@rm -f $(TARGET)
	@echo "🧹 Clean complete!"

# Install dependencies (Ubuntu/Debian)
.PHONY: install-deps
install-deps:
	@echo "📦 Installing SDL2 dependencies..."
	sudo apt update
	sudo apt install -y libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-gfx-dev

# Development build with debug info
.PHONY: debug
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Help
.PHONY: help
help:
	@echo "🐍 Snake SDL2 - Revolutionary Edition Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all           - Build the game (default)"
	@echo "  run           - Build and run the game"
	@echo "  clean         - Remove build files"
	@echo "  debug         - Build with debug information"
	@echo "  install-deps  - Install SDL2 dependencies"
	@echo "  help          - Show this help"