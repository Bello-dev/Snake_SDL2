#include "fonction_bell.h"

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));


    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    Mix_Music *mange;
    Mix_Music *music;
    Mix_Music *song[30] = {NULL};    

    if (!init(&window, &renderer, &music, &mange, song)) 
        return EXIT_FAILURE;
            // Mix_PlayMusic(music, -1);

       // while(Mix_PlayingMusic());

    if (!load_food_texture(renderer)) 
    {
        cleanup(window, renderer, music, mange, song);
        return EXIT_FAILURE;
    }

    game_loop(renderer, &music, &mange, song);

    cleanup(window, renderer, music, mange, song);

    return EXIT_SUCCESS;
}