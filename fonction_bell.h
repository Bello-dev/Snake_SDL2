#ifndef __FONCTION_BELL
#define __FONCTION_BELL

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define RADIUS 10
#define FOOD_SIZE 32

typedef struct coordonne {
    int x;
    int y;
} coordonne;

typedef struct Serpent {
    coordonne cercle;
    struct Serpent *next;
} Serpent;

// Variables globales
extern int taille_serpent;
extern int VITESSE;
extern SDL_Texture *foodTexture;
extern coordonne food;

int init(SDL_Window **window, SDL_Renderer **renderer, Mix_Music **music,  Mix_Music **choc);
double distance(int x1, int y1, int x2, int y2);
void afficher_food(SDL_Renderer *renderer) ;
void generate_food();
Serpent *add_queue(Serpent *s);
int load_food_texture(SDL_Renderer *renderer);
void afficher_serpent(SDL_Renderer *renderer, Serpent *s);
void update_snake(Serpent *s, int dx, int dy);
int check_collision(Serpent *s);
void dessiner_bordure_blocs(SDL_Renderer *renderer);
void game_loop(SDL_Renderer *renderer,  Mix_Music *music,  Mix_Music **choc);
void cleanup(SDL_Window *window, SDL_Renderer *renderer, Mix_Music *music, Mix_Music *choc); 













#endif