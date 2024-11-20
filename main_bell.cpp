#include "fonction_bell.h"

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));


    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    Mix_Music *mange;
    Mix_Music *music;
    Mix_Music *song[30] = {NULL}; 
    TTF_Init();   

    if (!init(&window, &renderer, &music, &mange, song)) 
        return EXIT_FAILURE;
            Mix_PlayMusic(music, -1);
            SDL_Delay(1000);
       // while(Mix_PlayingMusic());

    if (!load_food_texture(renderer)) 
    {
        cleanup(window, renderer, music, mange, song);
        return EXIT_FAILURE;
    }
    chargement(renderer, chargementTexture, song);

    game_loop(renderer, &music, &mange, song);

    cleanup(window, renderer, music, mange, song);

    return EXIT_SUCCESS;
}