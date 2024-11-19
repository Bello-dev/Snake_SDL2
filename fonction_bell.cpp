#include "fonction_bell.h"

int taille_serpent = 0;
int VITESSE = 90;
SDL_Texture *foodTexture = NULL;
coordonne food;


double distance(int x1, int y1, int x2, int y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

int init(SDL_Window **window, SDL_Renderer **renderer, Mix_Music **music,  Mix_Music **choc)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
    {
        cout<<"Erreur SDL: " <<SDL_GetError()<<"\n";
        return 0;
    }
    
    *window = SDL_CreateWindow("Jeu du Serpent", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH+200, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    
    if (!*window) 
    {
        cout<<"Erreur création fenêtre: "<< SDL_GetError()<<"\n";
        return 0;
    }
   
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    
    if (!*renderer) 
    {
        cout<<"Erreur création renderer: "<< SDL_GetError()<<"\n";
        return 0;
    }

    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3)
    {
        cout<<"Erreur initialisation audio "<<Mix_GetError()<<"\n";
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cout<<"Erreur Mix_OpenAudio "<<Mix_GetError()<<"\n";
        return 0;
    }

    *music = Mix_LoadMUS("data/sons/music.mp3");

    if (! *music)
    {
        cout<<"Erreur lecture musique "<<Mix_GetError()<<"\n";
        return 0;
    }
    else
	    {
	        Mix_PlayMusic(*music, -1);
	    }

    *choc = Mix_LoadMUS("data/sons/choc.mp3");

    if (! *choc)
    {
        cout<<"Erreur lecture musique "<<Mix_GetError()<<"\n";
        return 0;
    }

    return 1;
}

int load_food_texture(SDL_Renderer *renderer) 
{
    SDL_Surface *foodSurface = IMG_Load("data/images/nouriture.jpg"); // Remplacez par votre chemin d'image
    
    if (!foodSurface) 
    {
        cout<<"Erreur chargement image nourriture: "<<IMG_GetError()<<"\n";
        return 0;
    }

    foodTexture = SDL_CreateTextureFromSurface(renderer, foodSurface);

    SDL_FreeSurface(foodSurface);

    if (!foodTexture) 
    {
        cout<<"Erreur création texture nourriture: "<<SDL_GetError()<<"\n";
        return 0;
    }

    return 1;
}

void afficher_food(SDL_Renderer *renderer) 
{
    SDL_Rect dstRect = {food.x - FOOD_SIZE / 2, food.y - FOOD_SIZE / 2, FOOD_SIZE, FOOD_SIZE};
    SDL_RenderCopy(renderer, foodTexture, NULL, &dstRect);
}

void generate_food() 
{
    food.x = rand() % (SCREEN_WIDTH - FOOD_SIZE) + FOOD_SIZE / 2;
    food.y = rand() % (SCREEN_HEIGHT - FOOD_SIZE) + FOOD_SIZE / 2;
}

Serpent *add_queue(Serpent *s) 
{
    if (!s) return NULL;

    Serpent *courant = s;
    while (courant->next) {
        courant = courant->next;
    }

    Serpent *nouveau = (Serpent *)malloc(sizeof(Serpent));
    if (!nouveau) {
        cout<<"Erreur allocation mémoire"<<"\n";
        exit(EXIT_FAILURE);
    }

    nouveau->cercle.x = courant->cercle.x;
    nouveau->cercle.y = courant->cercle.y;
    nouveau->next = NULL;

    courant->next = nouveau;
    taille_serpent++;

    return s;
}

void afficher_serpent(SDL_Renderer *renderer, Serpent *s) 
{
    Serpent *courant = s;
    while (courant) 
    {
        filledCircleRGBA(renderer, courant->cercle.x, courant->cercle.y, RADIUS, 0, 255, 0, 255);
        courant = courant->next;
    }
}

void update_snake(Serpent *s, int dx, int dy) 
{
    Serpent *courant = s;
    int prevX = courant->cercle.x;
    int prevY = courant->cercle.y;
    courant->cercle.x += dx;
    courant->cercle.y += dy;

    courant = courant->next;
    while (courant) {
        int tempX = courant->cercle.x;
        int tempY = courant->cercle.y;
        courant->cercle.x = prevX;
        courant->cercle.y = prevY;
        prevX = tempX;
        prevY = tempY;
        courant = courant->next;
    }
}

int check_collision(Serpent *s) 
{
    if (!s) return 0;

    // Collision avec les murs
    if (s->cercle.x - RADIUS < 20 || s->cercle.y - RADIUS < 20 ||
        s->cercle.x + RADIUS > SCREEN_WIDTH - 20 || s->cercle.y + RADIUS > SCREEN_HEIGHT - 20) 
    {
        return 1;
    }

    // Collision avec le corps
    Serpent *courant = s->next;

    while (courant) {
        if (distance(s->cercle.x, s->cercle.y, courant->cercle.x, courant->cercle.y) < 2 * RADIUS && taille_serpent != 1) 
        {
            return 1;
        }

        courant = courant->next;
    }

    return 0;
}

void dessiner_bordure_blocs(SDL_Renderer *renderer) 
{
    int block_size = 20;
    SDL_Rect rect;

    for (int x = 0; x < SCREEN_WIDTH; x += block_size) 
    {
        // Haut
        rect = (SDL_Rect){x, 0, block_size, block_size};
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // Couleur bleu clair
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_RenderCopy(renderer, foodTexture, NULL, &rect);

        // Bas
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); // Couleur bleu clair

        rect = (SDL_Rect){x, SCREEN_HEIGHT - block_size, block_size, block_size};
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_RenderCopy(renderer, foodTexture, NULL, &rect);


    }

    // Dessiner les blocs à gauche et à droite
    for (int y = 0; y < SCREEN_HEIGHT; y += block_size) 
    {
        // Gauche
        rect = (SDL_Rect){0, y, block_size, block_size};
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_RenderCopy(renderer, foodTexture, NULL, &rect);


        // Droite
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

        rect = (SDL_Rect){SCREEN_WIDTH - block_size, y, block_size, block_size};
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_RenderCopy(renderer, foodTexture, NULL, &rect);

    }
}

// Boucle principale du jeu
void game_loop(SDL_Renderer *renderer,  Mix_Music **music,  Mix_Music **choc) 
{
    Serpent *serpent = (Serpent *)malloc(sizeof(Serpent));
    
    if (!serpent) 
    {
        cout<<"Erreur allocation mémoire\n";
        exit(EXIT_FAILURE);
    }

    // Initialisation du serpent
    serpent->cercle.x = SCREEN_WIDTH / 2;
    serpent->cercle.y = SCREEN_HEIGHT / 2;
    serpent->next = NULL;

    int dx = RADIUS * 2, dy = 0;
    generate_food();

    int running = 1;

    SDL_Event event;

    while (running) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
                running = 0;

            if (event.type == SDL_KEYDOWN) 
            {
                switch (event.key.keysym.sym) 
                {
                    case SDLK_UP:
                        if (dy == 0) {
                            dx = 0;
                            dy = -RADIUS * 2;
                        }
                        break;

                    case SDLK_DOWN:
                        if (dy == 0) {
                            dx = 0;
                            dy = RADIUS * 2;
                        }
                        break;

                    case SDLK_LEFT:
                        if (dx == 0) {
                            dx = -RADIUS * 2;
                            dy = 0;
                        }
                        break;

                    case SDLK_RIGHT:
                        if (dx == 0) {
                            dx = RADIUS * 2;
                            dy = 0;
                        }
                        break;
                }
            }
        }

        // Mise à jour
        update_snake(serpent, dx, dy);

        // Mix_PlayMusic(music, 1);
        // Mix_PlayingMusic();
        // Mix_Volume(2, 100);
        // Mix_Playing(1);

        if (distance(serpent->cercle.x, serpent->cercle.y, food.x, food.y) < RADIUS + FOOD_SIZE / 2) 
        {
        	 Mix_PlayMusic(*choc, 1);

            serpent = add_queue(serpent);
            generate_food();
        }

        if (check_collision(serpent)) 
        {
            printf("Game Over! Taille du serpent: %d\n", taille_serpent);
            // SDL_Delay(2000);
            // running = 0;
        }

        // Affichage

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        dessiner_bordure_blocs(renderer);
        afficher_food(renderer);
        afficher_serpent(renderer, serpent);

        SDL_RenderPresent(renderer);

        SDL_Delay(VITESSE);
    }

    while (serpent) 
    {
        Serpent *temp = serpent;
        serpent = serpent->next;
        free(temp);
    }
}

void cleanup(SDL_Window *window, SDL_Renderer *renderer, Mix_Music *music, Mix_Music *choc) 
{
    if (foodTexture) 
        SDL_DestroyTexture(foodTexture);

    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);

    if(music)
        Mix_FreeMusic(music);

    if (choc)
        Mix_FreeMusic(choc);

    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

void renderTextCentered(SDL_Renderer *renderer, const char *text, TTF_Font *font, SDL_Color textColor, SDL_Rect *rect) 
{
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, textColor);
    if (!textSurface) {
        cout<<"Erreur création surface : "<<TTF_GetError()<<"\n";
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    
    if (!textTexture) 
    {
        cout<<"Erreur création texture : "<<SDL_GetError()<<"\n";
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    SDL_Rect textRect;
    textRect.x = rect->x + (rect->w - textWidth) / 2;
    textRect.y = rect->y + (rect->h - textHeight) / 2;
    textRect.w = textWidth;
    textRect.h = textHeight;

    SDL_FreeSurface(textSurface);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, rect);

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
}

void ecran_lateral(SDL_Renderer *renderer, TTF_Font *font, SDL_Color textColor)
{
	SDL_Rect lat={SCREEN_WIDTH, 0, 200, SCREEN_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 190, 80, 255, 255);
    SDL_RenderFillRect(renderer, &lat);
    SDL_Color color = {190, 80, 255, 255};

   renderTextCentered(renderer, "BELLO DEV", font, color, &lat);

}