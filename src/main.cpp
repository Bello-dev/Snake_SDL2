#include "game.h"

int main(int argc, char* argv[]) {
    // Create game instance
    Game game;
    
    // Initialize game
    if (!game.init()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Run game
    game.run();
    
    return EXIT_SUCCESS;
}