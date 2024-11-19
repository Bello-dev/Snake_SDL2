#include "fonction_bell.h"

int taille_serpent = 0;
int VITESSE = 90;
SDL_Texture *cadreTexture = NULL;
SDL_Texture *foodTexture = NULL;
SDL_Texture *cadreLateralTexture = NULL;
SDL_Texture *bgTexture = NULL;

coordonne food;


double distance(int x1, int y1, int x2, int y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

int init(SDL_Window **window, SDL_Renderer **renderer, Mix_Music **music,  Mix_Music **choc, Mix_Music *song[])
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

    for (int i = 0; i < 30; i++)
    {
	char nomFichier[255];
    	sprintf(nomFichier,"data/sons/Sound/common_ %d.wav",i+1);

  	  	song[i] = Mix_LoadMUS(nomFichier);

	    if (! song[i])
	    {
	        cout<<"Erreur lecture musique "<<nomFichier<<Mix_GetError()<<"\n";
	        return 0;
	    }
	}
    return 1;
}

int load_food_texture(SDL_Renderer *renderer) 
{
    SDL_Surface *cadreSurface = IMG_Load("data/images/murail.jpg");
    
    if (!cadreSurface) 
    {
        cout<<"Erreur chargement image cadre: "<<IMG_GetError()<<"\n";
        return 0;
    }

    cadreTexture = SDL_CreateTextureFromSurface(renderer, cadreSurface);

    SDL_FreeSurface(cadreSurface);

    if (!cadreTexture) 
    {
        cout<<"Erreur création texture cadre: "<<SDL_GetError()<<"\n";
        return 0;
    }

	SDL_Surface *foodSurface = IMG_Load("data/images/nouriture.jpg");
    
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

    SDL_Surface *bgSurface = IMG_Load("data/images/bg.jpg");
    
    if (!bgSurface) 
    {
        cout<<"Erreur chargement image badgrund: "<<IMG_GetError()<<"\n";
        return 0;
    }

    bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);

    SDL_FreeSurface(bgSurface);

    if (!bgTexture) 
    {
        cout<<"Erreur création texture badgrund: "<<SDL_GetError()<<"\n";
        return 0;
    }

    SDL_Surface *cadreLatSurface = IMG_Load("data/images/bgLat.jpg");
    
    if (!cadreLatSurface) 
    {
        cout<<"Erreur chargement image bgLat: "<<IMG_GetError()<<"\n";
        return 0;
    }

    cadreLateralTexture = SDL_CreateTextureFromSurface(renderer, cadreLatSurface);

    SDL_FreeSurface(cadreLatSurface);

    if (!cadreLateralTexture) 
    {
        cout<<"Erreur création texture bgLat: "<<SDL_GetError()<<"\n";
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
        SDL_RenderCopy(renderer, cadreTexture, NULL, &rect);

        // Bas
        rect = (SDL_Rect){x, SCREEN_HEIGHT - block_size, block_size, block_size};
        SDL_RenderCopy(renderer, cadreTexture, NULL, &rect);
    }

    // Dessiner les blocs principals à gauche et à droite
    for (int y = 0; y < SCREEN_HEIGHT; y += block_size) 
    {
        // Gauche
        rect = (SDL_Rect){0, y, block_size, block_size};
        SDL_RenderCopy(renderer, cadreTexture, NULL, &rect);


        // Droite
        rect = (SDL_Rect){SCREEN_WIDTH - block_size, y, block_size, block_size};
        SDL_RenderCopy(renderer, cadreTexture, NULL, &rect);
    }

	// Dessiner les blocs sur l'ecran lateral  à gauche et à droite
    for (int y = 0; y < SCREEN_HEIGHT; y += block_size) 
    {
        // Gauche
        rect = (SDL_Rect){SCREEN_WIDTH, y, block_size, block_size};
        SDL_RenderCopy(renderer, cadreLateralTexture, NULL, &rect);


        // Droite
        rect = (SDL_Rect){SCREEN_WIDTH+200 - block_size, y, block_size, block_size};
        SDL_RenderCopy(renderer, cadreLateralTexture, NULL, &rect);
    } 

     for (int x = SCREEN_WIDTH; x < SCREEN_WIDTH+200; x += block_size) 
    {
        // Haut
        rect = (SDL_Rect){x, 0, block_size, block_size};
        SDL_RenderCopy(renderer, cadreLateralTexture, NULL, &rect);

        // Bas
        rect = (SDL_Rect){x, SCREEN_HEIGHT - block_size, block_size, block_size};
        SDL_RenderCopy(renderer, cadreLateralTexture, NULL, &rect);
    }   
}

// Boucle principale du jeu
void game_loop(SDL_Renderer *renderer,  Mix_Music **music,  Mix_Music **choc, Mix_Music *song[]) 
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
    SDL_Rect bg = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect bgLat = {SCREEN_WIDTH, 0, 200, SCREEN_HEIGHT};
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
                    	if(song[4])
						    {
						    	Mix_PlayMusic(song[4], 1);
						    	// while(Mix_PlayingMusic());
						    }
                        if (dy == 0) {
                            dx = 0;
                            dy = -RADIUS * 2;
                        }
                        break;

                    case SDLK_DOWN:
                    	if(song[4])
                    	{
						   	Mix_PlayMusic(song[4], 1);
						    // while(Mix_PlayingMusic());
						}
                        if (dy == 0) {
                            dx = 0;
                            dy = RADIUS * 2;
                        }
                        break;

                    case SDLK_LEFT:
                    	if(song[4])
                    	{
						    Mix_PlayMusic(song[4], 1);
						    // while(Mix_PlayingMusic());
						}
                        if (dx == 0) {
                            dx = -RADIUS * 2;
                            dy = 0;
                        }
                        break;

                    case SDLK_RIGHT:
                    	if(song[4])
                    	{
						    Mix_PlayMusic(song[4], 1);
						    // while(Mix_PlayingMusic());
						}
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
        	 // Mix_PlayMusic(*choc, 1);
        	if(song[26])
        	{
				Mix_PlayMusic(song[26], 1);
				// while(Mix_PlayingMusic());
			}

            serpent = add_queue(serpent);
            generate_food();
        }

        if (check_collision(serpent)) 
        {
            printf("Game Over! Taille du serpent: %d\n", taille_serpent);
            if(song[21])
		    {
		    	Mix_PlayMusic(song[21], 1);
		    	// while(Mix_PlayingMusic());
		    }
            // SDL_Delay(2000);
            running = 0;
        }

        // Affichage

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, bgTexture, NULL, &bg);
        // SDL_RenderCopy(renderer, cadreLateralTexture, NULL, &bgLat);

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

void cleanup(SDL_Window *window, SDL_Renderer *renderer, Mix_Music *music, Mix_Music *choc, Mix_Music *song[]) 
{
    if (cadreTexture) 
        SDL_DestroyTexture(cadreTexture);

    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);

    if(music)
        Mix_FreeMusic(music);

    if (choc)
        Mix_FreeMusic(choc);

    for (int i = 0; i < 30; ++i)
    {
    	if (song[i])
    	{
    		Mix_FreeMusic(song[i]);
    		song[i] = NULL;
    	}
    }

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