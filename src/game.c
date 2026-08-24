#include "game.h"

bool game_init(Game* game) {
    // Create window
    game->window = SDL_CreateWindow("Snake SDL2 - Enhanced Edition", 
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!game->window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Create renderer
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game->renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Load font (we'll use a basic font for now)
    game->font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", 24);
    game->large_font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf", 48);
    
    // If system fonts don't exist, create without fonts (text will be skipped)
    if (!game->font) {
        printf("Warning: Could not load font. Text will not be displayed.\n");
    }
    if (!game->large_font) {
        printf("Warning: Could not load large font. Large text will not be displayed.\n");
    }
    
    // Initialize audio (optional - game works without sound)
    game->eat_sound = NULL;
    game->game_over_sound = NULL;
    
    // Initialize game state
    game->state = GAME_MENU;
    game->difficulty = DIFFICULTY_NORMAL;
    game->score = 0;
    game->high_score = 0;
    game->running = true;
    game->last_move_time = 0;
    game->move_delay = 200; // milliseconds
    game->base_move_delay = 200;
    game->food_pulse = 0.0f;
    game->game_over_alpha = 0;
    game->particle_count = 0;
    game->foods_eaten = 0;
    game->game_start_time = SDL_GetTicks();
    
    // Initialize game objects
    snake_init(&game->snake);
    food_spawn(&game->food, &game->snake);
    
    // Set random seed
    srand(time(NULL));
    
    return true;
}

void game_cleanup(Game* game) {
    if (game->eat_sound) {
        Mix_FreeChunk(game->eat_sound);
    }
    if (game->game_over_sound) {
        Mix_FreeChunk(game->game_over_sound);
    }
    if (game->font) {
        TTF_CloseFont(game->font);
    }
    if (game->large_font) {
        TTF_CloseFont(game->large_font);
    }
    if (game->renderer) {
        SDL_DestroyRenderer(game->renderer);
    }
    if (game->window) {
        SDL_DestroyWindow(game->window);
    }
}

void game_handle_events(Game* game) {
    SDL_Event e;
    
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            game->running = false;
        }
        else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    game->running = false;
                    break;
                    
                case SDLK_SPACE:
                    if (game->state == GAME_MENU) {
                        game->state = GAME_PLAYING;
                        game_reset(game);
                    }
                    else if (game->state == GAME_PLAYING) {
                        game->state = GAME_PAUSED;
                    }
                    else if (game->state == GAME_PAUSED) {
                        game->state = GAME_PLAYING;
                    }
                    else if (game->state == GAME_OVER) {
                        game->state = GAME_MENU;
                    }
                    break;
                    
                case SDLK_1:
                    if (game->state == GAME_MENU) {
                        game->difficulty = DIFFICULTY_EASY;
                    }
                    break;
                    
                case SDLK_2:
                    if (game->state == GAME_MENU) {
                        game->difficulty = DIFFICULTY_NORMAL;
                    }
                    break;
                    
                case SDLK_3:
                    if (game->state == GAME_MENU) {
                        game->difficulty = DIFFICULTY_HARD;
                    }
                    break;
                    
                case SDLK_UP:
                case SDLK_w:
                    if (game->state == GAME_PLAYING) {
                        snake_change_direction(&game->snake, DIR_UP);
                    }
                    break;
                    
                case SDLK_DOWN:
                case SDLK_s:
                    if (game->state == GAME_PLAYING) {
                        snake_change_direction(&game->snake, DIR_DOWN);
                    }
                    break;
                    
                case SDLK_LEFT:
                case SDLK_a:
                    if (game->state == GAME_PLAYING) {
                        snake_change_direction(&game->snake, DIR_LEFT);
                    }
                    break;
                    
                case SDLK_RIGHT:
                case SDLK_d:
                    if (game->state == GAME_PLAYING) {
                        snake_change_direction(&game->snake, DIR_RIGHT);
                    }
                    break;
                    
                case SDLK_r:
                    if (game->state == GAME_OVER) {
                        game->state = GAME_PLAYING;
                        game_reset(game);
                    }
                    break;
            }
        }
    }
}

void game_update(Game* game) {
    if (game->state != GAME_PLAYING) {
        // Update visual effects even when not playing
        game->food_pulse += 0.1f;
        if (game->food_pulse > 2 * M_PI) {
            game->food_pulse = 0;
        }
        
        if (game->state == GAME_OVER && game->game_over_alpha < 255) {
            game->game_over_alpha += 5;
            if (game->game_over_alpha > 255) game->game_over_alpha = 255;
        }
        
        return;
    }
    
    Uint32 current_time = SDL_GetTicks();
    
    // Update visual effects
    game->food_pulse += 0.15f;
    if (game->food_pulse > 2 * M_PI) {
        game->food_pulse = 0;
    }
    
    // Update particles
    update_particles(game);
    
    // Move snake based on timing
    if (current_time - game->last_move_time >= game->move_delay) {
        snake_move(&game->snake);
        game->last_move_time = current_time;
        
        // Check collision with walls or self
        if (snake_check_collision(&game->snake)) {
            game->state = GAME_OVER;
            game->game_over_alpha = 0;
            if (game->score > game->high_score) {
                game->high_score = game->score;
            }
            if (game->game_over_sound) {
                Mix_PlayChannel(-1, game->game_over_sound, 0);
            }
            return;
        }
        
        // Check collision with food
        if (food_check_collision(&game->food, &game->snake)) {
            // Spawn particles at food location
            spawn_food_particles(game, game->food.x * GRID_SIZE + GRID_SIZE/2, 
                                       game->food.y * GRID_SIZE + GRID_SIZE/2);
            
            snake_grow(&game->snake);
            food_spawn(&game->food, &game->snake);
            game->foods_eaten++;
            
            // Calculate score based on difficulty and length
            int base_score = 10;
            switch (game->difficulty) {
                case DIFFICULTY_EASY: base_score = 5; break;
                case DIFFICULTY_NORMAL: base_score = 10; break;
                case DIFFICULTY_HARD: base_score = 20; break;
            }
            
            // Bonus for length
            int length_bonus = (game->snake.length - 3) * 2;
            game->score += base_score + length_bonus;
            
            // Increase speed slightly based on difficulty
            int speed_increase = 0;
            switch (game->difficulty) {
                case DIFFICULTY_EASY: speed_increase = 1; break;
                case DIFFICULTY_NORMAL: speed_increase = 2; break;
                case DIFFICULTY_HARD: speed_increase = 3; break;
            }
            
            if (game->move_delay > 60) {
                game->move_delay -= speed_increase;
            }
            
            if (game->eat_sound) {
                Mix_PlayChannel(-1, game->eat_sound, 0);
            }
        }
    }
}

void game_render(Game* game) {
    // Clear screen with gradient background
    render_gradient_background(game->renderer);
    
    if (game->state == GAME_MENU) {
        render_menu(game);
    }
    else if (game->state == GAME_PLAYING || game->state == GAME_PAUSED) {
        // Render game objects
        render_food(game->renderer, &game->food, game->food_pulse);
        render_snake(game->renderer, &game->snake);
        render_particles(game);
        render_ui(game);
        
        if (game->state == GAME_PAUSED) {
            // Render pause overlay
            SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 150);
            SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderFillRect(game->renderer, &overlay);
        }
    }
    else if (game->state == GAME_OVER) {
        // Render game objects with overlay
        render_food(game->renderer, &game->food, game->food_pulse);
        render_snake(game->renderer, &game->snake);
        render_particles(game);
        render_ui(game);
        render_game_over(game);
    }
    
    SDL_RenderPresent(game->renderer);
}

void game_reset(Game* game) {
    snake_init(&game->snake);
    food_spawn(&game->food, &game->snake);
    game->score = 0;
    game->foods_eaten = 0;
    game->game_start_time = SDL_GetTicks();
    
    // Set initial speed based on difficulty
    switch (game->difficulty) {
        case DIFFICULTY_EASY: 
            game->base_move_delay = 250;
            break;
        case DIFFICULTY_NORMAL: 
            game->base_move_delay = 200;
            break;
        case DIFFICULTY_HARD: 
            game->base_move_delay = 150;
            break;
    }
    
    game->move_delay = game->base_move_delay;
    game->last_move_time = 0;
    game->food_pulse = 0;
    game->game_over_alpha = 0;
    game->particle_count = 0;
}