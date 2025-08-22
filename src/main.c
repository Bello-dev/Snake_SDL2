#include "game.h"

int main(int argc, char* args[]) {
    (void)argc; // Suppress unused parameter warning
    (void)args; // Suppress unused parameter warning
    
    Game game;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    
    // Initialize TTF
    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }
    
    // Initialize SDL_mixer (optional - continue without sound if it fails)
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Warning: SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        printf("Game will continue without sound.\n");
    }
    
    // Initialize game
    if (!game_init(&game)) {
        printf("Failed to initialize game!\n");
        Mix_Quit();
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    
    // Main game loop
    while (game.running) {
        game_handle_events(&game);
        game_update(&game);
        game_render(&game);
        
        // Cap frame rate to 60 FPS
        SDL_Delay(16);
    }
    
    // Cleanup
    game_cleanup(&game);
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}