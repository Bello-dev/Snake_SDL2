#include "fonction_bell.h"

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));


    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    Mix_Music *choc;
     Mix_Music *music;

    if (!init(&window, &renderer, &music, &choc)) {
        return EXIT_FAILURE;
    }
            // Mix_PlayMusic(music, -1);

       // while(Mix_PlayingMusic());


    if (!load_food_texture(renderer)) {
        cleanup(window, renderer, music, choc);
        return EXIT_FAILURE;
    }

    game_loop(renderer, music, &choc);

    cleanup(window, renderer, music, choc);

    return EXIT_SUCCESS;
}