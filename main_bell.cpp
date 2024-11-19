#include "fonction_bell.h"

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));


    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    Mix_Music *mange;
    Mix_Music *music;

    if (!init(&window, &renderer, &music, &mange)) 
        return EXIT_FAILURE;
            // Mix_PlayMusic(music, -1);

       // while(Mix_PlayingMusic());

    if (!load_food_texture(renderer)) {
        cleanup(window, renderer, music, mange);
        return EXIT_FAILURE;
    }

    game_loop(renderer, &music, &mange);

    cleanup(window, renderer, music, mange);

    return EXIT_SUCCESS;
}