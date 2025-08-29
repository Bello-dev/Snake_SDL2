#!/bin/bash

# Snake SDL2 - Revolutionary Edition Launcher
# BELLO DEV - 2024

echo "🐍 Snake SDL2 - Revolutionary Edition 🐍"
echo "=========================================="
echo ""

# Check if game is compiled
if [ ! -f "data/snake_revolutionary" ]; then
    echo "❌ Game not found! Compiling now..."
    make all
    if [ $? -ne 0 ]; then
        echo "❌ Compilation failed! Please check dependencies."
        echo "Run: make install-deps"
        exit 1
    fi
fi

echo "🎮 Starting Revolutionary Snake Game..."
echo ""
echo "🎯 Controls:"
echo "   WASD / Arrow Keys - Move snake"  
echo "   1/2/3 - Select difficulty"
echo "   SPACE/ENTER - Start game"
echo "   P/ESC - Pause/Menu"
echo ""
echo "🍎 Magic Foods:"
echo "   🟢 Normal: +10 points, grows snake"
echo "   🟡 Speed: +60% speed for 5s"  
echo "   🔵 Double: x2 score for 8s"
echo "   🟠 Golden: +30 points bonus"
echo "   🟣 Shrink: -2 segments (escape tool)"
echo "   💜 Phase: Walk through walls for 6s"
echo "   🔴 MEGA: +50 points + screen shake!"
echo ""
echo "🏆 Tips:"
echo "   - Chain foods within 3s for combos"
echo "   - Stack power-ups for mega scores"  
echo "   - Phase mode saves you from tight spots"
echo "   - MEGA foods are rare (2%) - treasure them!"
echo ""
echo "Good luck, and may the RNG be with you! ✨"
echo "=========================================="
echo ""

# Set environment for better compatibility
export SDL_AUDIODRIVER=pulse

# Launch game in data directory
cd data && ./snake_revolutionary

echo ""
echo "Thanks for playing Snake Revolutionary Edition!"
echo "🐍 Remember: Practice makes perfect! 🐍"