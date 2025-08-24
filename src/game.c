#include "game.h"

#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    #include <string.h>
#endif

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
    
    // Load fonts using cross-platform font loading
    game->font = load_system_font(24);
    game->large_font = load_system_font(48);
    
    // If system fonts don't exist, create without fonts (text will be skipped)
    if (!game->font) {
        printf("Warning: Could not load system font. Text will not be displayed.\n");
    }
    if (!game->large_font) {
        printf("Warning: Could not load large system font. Large text will not be displayed.\n");
    }
    
    // Initialize audio (optional - game works without sound)
    game->eat_sound = NULL;
    game->game_over_sound = NULL;
    
    // Initialize game state
    game->state = GAME_MENU;
    game->difficulty = DIFFICULTY_NORMAL;
    game->score = 0;
    game->high_score = load_high_score(); // Load from file
    game->running = true;
    game->last_move_time = 0;
    game->move_delay = 200; // milliseconds
    game->base_move_delay = 200;
    game->food_pulse = 0.0f;
    game->game_over_alpha = 0;
    game->particle_count = 0;
    game->foods_eaten = 0;
    game->special_foods_eaten = 0;
    game->game_start_time = SDL_GetTicks();
    game->level = 1;
    game->foods_needed_for_level = 5;
    game->base_score_per_food = 10;
    game->screen_shake_intensity = 0.0f;
    game->screen_shake_end_time = 0;
    
    // Initialize power-up system
    powerups_init(&game->power_ups);
    
    // Initialize game objects
    snake_init(&game->snake);
    food_spawn(&game->food, &game->snake);
    game->food.type = food_get_random_type(game->level, game->foods_eaten);
    
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
                    
                // Debug/cheat keys (for testing)
                case SDLK_p:
                    if (game->state == GAME_PLAYING) {
                        // Add particles for fun
                        for (int i = 0; i < 20; i++) {
                            float angle = (rand() / (float)RAND_MAX) * 2 * M_PI;
                            float speed = 3.0f + (rand() % 3);
                            add_particle(game, SCREEN_WIDTH/2, SCREEN_HEIGHT/2,
                                        cosf(angle) * speed, sinf(angle) * speed,
                                        255, 100 + rand() % 155, 100 + rand() % 155);
                        }
                    }
                    break;
            }
        }
    }
}

void game_update(Game* game) {
    Uint32 current_time = SDL_GetTicks();
    
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
    
    // Update power-ups
    powerups_update(&game->power_ups, current_time);
    
    // Update visual effects
    game->food_pulse += 0.15f;
    if (game->food_pulse > 2 * M_PI) {
        game->food_pulse = 0;
    }
    
    // Update food effects
    if (game->food.active) {
        game->food.pulse_phase += 0.2f;
        if (game->food.pulse_phase > 2 * M_PI) {
            game->food.pulse_phase = 0;
        }
        game->food.glow_intensity = 0.8f + 0.2f * sinf(game->food.pulse_phase);
    }
    
    // Update particles
    update_particles(game);
    
    // Update screen shake
    if (current_time < game->screen_shake_end_time) {
        float shake_progress = (float)(game->screen_shake_end_time - current_time) / 500.0f;
        game->screen_shake_intensity *= shake_progress;
    } else {
        game->screen_shake_intensity = 0.0f;
    }
    
    // Determine current move delay based on power-ups and level
    Uint32 move_delay = level_get_speed(game->level, powerups_is_speed_active(&game->power_ups));
    
    // Move snake based on timing
    if (current_time - game->last_move_time >= move_delay) {
        snake_move(&game->snake);
        game->last_move_time = current_time;
        
        // Check collision with walls or self (with phase power-up consideration)
        bool collision = false;
        if (powerups_is_phase_active(&game->power_ups)) {
            // Check self collision only when phasing
            Segment* head = &game->snake.segments[0];
            for (int i = 1; i < game->snake.length; i++) {
                if (head->x == game->snake.segments[i].x && head->y == game->snake.segments[i].y) {
                    collision = true;
                    break;
                }
            }
            // Wrap around screen when phasing through walls
            if (head->x < 0) head->x = GRID_WIDTH - 1;
            if (head->x >= GRID_WIDTH) head->x = 0;
            if (head->y < 0) head->y = GRID_HEIGHT - 1;
            if (head->y >= GRID_HEIGHT) head->y = 0;
        } else {
            collision = snake_check_collision(&game->snake);
        }
        
        if (collision) {
            game->state = GAME_OVER;
            game->game_over_alpha = 0;
            if (game->score > game->high_score) {
                game->high_score = game->score;
                save_high_score(game->high_score); // Save new high score
            }
            if (game->game_over_sound) {
                Mix_PlayChannel(-1, game->game_over_sound, 0);
            }
            return;
        }
        
        // Check collision with food
        if (food_check_collision(&game->food, &game->snake)) {
            // Spawn particles at food location with type-specific effects
            spawn_food_particles(game, game->food.x * GRID_SIZE + GRID_SIZE/2, 
                                       game->food.y * GRID_SIZE + GRID_SIZE/2, game->food.type);
            spawn_power_up_particles(game, game->food.x * GRID_SIZE + GRID_SIZE/2,
                                           game->food.y * GRID_SIZE + GRID_SIZE/2, game->food.type);
            
            // Apply food effect (includes growing snake and updating score)
            food_apply_effect(game, game->food.type);
            
            // Spawn new food
            food_spawn(&game->food, &game->snake);
            game->food.type = food_get_random_type(game->level, game->foods_eaten);
            
            // Check for level up
            if (game->foods_eaten >= level_get_required_foods(game->level)) {
                level_up(game);
            }
            
            if (game->eat_sound) {
                Mix_PlayChannel(-1, game->eat_sound, 0);
            }
        }
    }
}

void game_render(Game* game) {
    // Apply screen shake if active
    int shake_x = 0, shake_y = 0;
    if (game->screen_shake_intensity > 0) {
        shake_x = (rand() % (int)(game->screen_shake_intensity * 2)) - (int)game->screen_shake_intensity;
        shake_y = (rand() % (int)(game->screen_shake_intensity * 2)) - (int)game->screen_shake_intensity;
    }
    
    // Clear screen with gradient background
    render_gradient_background(game->renderer);
    
    // Apply shake offset
    if (shake_x != 0 || shake_y != 0) {
        SDL_Rect viewport;
        SDL_RenderGetViewport(game->renderer, &viewport);
        viewport.x += shake_x;
        viewport.y += shake_y;
        SDL_RenderSetViewport(game->renderer, &viewport);
    }
    
    if (game->state == GAME_MENU) {
        render_menu(game);
    }
    else if (game->state == GAME_PLAYING || game->state == GAME_PAUSED) {
        // Render game objects
        render_food(game->renderer, &game->food);
        render_snake(game->renderer, &game->snake, &game->power_ups);
        render_particles(game);
        render_power_up_indicators(game);
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
        render_food(game->renderer, &game->food);
        render_snake(game->renderer, &game->snake, &game->power_ups);
        render_particles(game);
        render_ui(game);
        render_game_over(game);
    }
    
    // Reset viewport if shake was applied
    if (shake_x != 0 || shake_y != 0) {
        SDL_Rect viewport = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderSetViewport(game->renderer, &viewport);
    }
    
    SDL_RenderPresent(game->renderer);
}

void game_reset(Game* game) {
    snake_init(&game->snake);
    food_spawn(&game->food, &game->snake);
    game->food.type = food_get_random_type(1, 0); // Start with level 1
    
    game->score = 0;
    game->foods_eaten = 0;
    game->special_foods_eaten = 0;
    game->level = 1;
    game->foods_needed_for_level = 5;
    game->base_score_per_food = 10;
    game->game_start_time = SDL_GetTicks();
    
    // Reset power-ups
    powerups_init(&game->power_ups);
    
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
    game->screen_shake_intensity = 0.0f;
    game->screen_shake_end_time = 0;
}

// Power-up system implementation
void powerups_init(PowerUps* power_ups) {
    power_ups->speed_boost = false;
    power_ups->double_score = false;
    power_ups->phase_through_walls = false;
    power_ups->speed_end_time = 0;
    power_ups->double_score_end_time = 0;
    power_ups->phase_end_time = 0;
    power_ups->combo_multiplier = 1;
    power_ups->combo_count = 0;
    power_ups->last_food_time = 0;
}

void powerups_update(PowerUps* power_ups, Uint32 current_time) {
    // Update speed boost
    if (power_ups->speed_boost && current_time >= power_ups->speed_end_time) {
        power_ups->speed_boost = false;
    }
    
    // Update double score
    if (power_ups->double_score && current_time >= power_ups->double_score_end_time) {
        power_ups->double_score = false;
    }
    
    // Update phase power
    if (power_ups->phase_through_walls && current_time >= power_ups->phase_end_time) {
        power_ups->phase_through_walls = false;
    }
    
    // Update combo system
    if (current_time - power_ups->last_food_time > 3000) { // 3 seconds timeout
        power_ups->combo_count = 0;
        power_ups->combo_multiplier = 1;
    }
}

bool powerups_is_speed_active(PowerUps* power_ups) {
    return power_ups->speed_boost;
}

bool powerups_is_double_score_active(PowerUps* power_ups) {
    return power_ups->double_score;
}

bool powerups_is_phase_active(PowerUps* power_ups) {
    return power_ups->phase_through_walls;
}

int powerups_get_score_multiplier(PowerUps* power_ups) {
    int multiplier = power_ups->combo_multiplier;
    if (power_ups->double_score) {
        multiplier *= 2;
    }
    return multiplier;
}

// Level system implementation
void level_up(Game* game) {
    game->level++;
    game->foods_needed_for_level = level_get_required_foods(game->level);
    
    // Increase base score per food
    game->base_score_per_food = 10 + (game->level - 1) * 2;
    
    // Add bonus score for level up
    game->score += game->level * 50;
    
    // Add screen shake for level up
    add_screen_shake(game, 10.0f, 800);
}

int level_get_required_foods(int level) {
    return 5 + (level - 1) * 3; // 5, 8, 11, 14, etc.
}

Uint32 level_get_speed(int level, bool speed_boost) {
    Uint32 base_delay = 200 - (level - 1) * 5; // Get faster each level
    if (base_delay < 80) base_delay = 80; // Minimum speed
    
    if (speed_boost) {
        return base_delay / 2; // Double speed when boosted
    }
    
    return base_delay;
}

// Screen shake utility
void add_screen_shake(Game* game, float intensity, Uint32 duration) {
    game->screen_shake_intensity = intensity;
    game->screen_shake_end_time = SDL_GetTicks() + duration;
}

// Save/Load system
void save_high_score(int score) {
    FILE* file = fopen("highscore.dat", "w");
    if (file) {
        fprintf(file, "%d", score);
        fclose(file);
    }
}

int load_high_score(void) {
    FILE* file = fopen("highscore.dat", "r");
    int score = 0;
    if (file) {
        if (fscanf(file, "%d", &score) != 1) {
            score = 0; // Default if reading fails
        }
        fclose(file);
    }
    return score;
}

// Cross-platform font loading implementation
const char* get_system_font_path(void) {
#ifdef PLATFORM_WINDOWS
    // Windows system fonts
    static char font_path[MAX_PATH];
    if (GetWindowsDirectory(font_path, sizeof(font_path))) {
        strcat(font_path, "\\Fonts\\arial.ttf");
        return font_path;
    }
    return NULL;
#elif defined(PLATFORM_ANDROID)
    // Android system fonts
    return "/system/fonts/Roboto-Regular.ttf";
#elif defined(PLATFORM_LINUX)
    // Try common Linux font locations
    static const char* linux_fonts[] = {
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 
        "/usr/share/fonts/TTF/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/System/Library/Fonts/Arial.ttf",  // macOS fallback
        NULL
    };
    
    for (int i = 0; linux_fonts[i] != NULL; i++) {
        FILE* test = fopen(linux_fonts[i], "r");
        if (test) {
            fclose(test);
            return linux_fonts[i];
        }
    }
    return NULL;
#else
    return NULL;
#endif
}

TTF_Font* load_system_font(int size) {
    const char* font_path = get_system_font_path();
    if (font_path) {
        TTF_Font* font = TTF_OpenFont(font_path, size);
        if (font) {
            return font;
        }
    }
    
    // If system font fails, try to create a basic font using TTF_OpenFont with NULL
    // This will use SDL2's built-in font rendering (if available)
    return NULL;
}