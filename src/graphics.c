#include "game.h"

void set_color(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void draw_rounded_rect(SDL_Renderer* renderer, int x, int y, int w, int h, int radius) {
    // For simplicity, we'll draw a regular rectangle
    // In a more advanced version, we could implement proper rounded corners
    (void)radius; // Suppress unused parameter warning
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void render_snake(SDL_Renderer* renderer, Snake* snake, PowerUps* power_ups) {
    for (int i = 0; i < snake->length; i++) {
        int x = snake->segments[i].x * GRID_SIZE;
        int y = snake->segments[i].y * GRID_SIZE;
        
        if (i == 0) {
            // Head - bright green with gradient effect and glow
            Uint8 head_color[] = COLOR_SNAKE_HEAD;
            
            // Special effects based on power-ups
            if (powerups_is_speed_active(power_ups)) {
                head_color[0] = 150; head_color[1] = 150; head_color[2] = 255; // Blue when speeding
            }
            if (powerups_is_phase_active(power_ups)) {
                head_color[0] = 255; head_color[1] = 150; head_color[2] = 255; // Purple when phasing
            }
            
            // Draw glow effect around head
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            int glow_size = powerups_is_speed_active(power_ups) ? 6 : 4;
            for (int glow = glow_size; glow > 0; glow--) {
                set_color(renderer, head_color[0], head_color[1], head_color[2], 30);
                SDL_Rect glow_rect = {x - glow, y - glow, GRID_SIZE + 2*glow, GRID_SIZE + 2*glow};
                SDL_RenderFillRect(renderer, &glow_rect);
            }
            
            set_color(renderer, head_color[0], head_color[1], head_color[2], head_color[3]);
            
            // Draw head with slightly larger size
            SDL_Rect head_rect = {x + 1, y + 1, GRID_SIZE - 2, GRID_SIZE - 2};
            SDL_RenderFillRect(renderer, &head_rect);
            
            // Add eyes based on direction
            set_color(renderer, 20, 20, 40, 255);
            int eye_size = 4;
            SDL_Rect left_eye, right_eye;
            
            switch (snake->direction) {
                case DIR_UP:
                    left_eye = (SDL_Rect){x + 5, y + 4, eye_size, eye_size};
                    right_eye = (SDL_Rect){x + GRID_SIZE - 9, y + 4, eye_size, eye_size};
                    break;
                case DIR_DOWN:
                    left_eye = (SDL_Rect){x + 5, y + GRID_SIZE - 8, eye_size, eye_size};
                    right_eye = (SDL_Rect){x + GRID_SIZE - 9, y + GRID_SIZE - 8, eye_size, eye_size};
                    break;
                case DIR_LEFT:
                    left_eye = (SDL_Rect){x + 4, y + 5, eye_size, eye_size};
                    right_eye = (SDL_Rect){x + 4, y + GRID_SIZE - 9, eye_size, eye_size};
                    break;
                case DIR_RIGHT:
                    left_eye = (SDL_Rect){x + GRID_SIZE - 8, y + 5, eye_size, eye_size};
                    right_eye = (SDL_Rect){x + GRID_SIZE - 8, y + GRID_SIZE - 9, eye_size, eye_size};
                    break;
            }
            
            SDL_RenderFillRect(renderer, &left_eye);
            SDL_RenderFillRect(renderer, &right_eye);
            
            // Add white highlight to eyes
            set_color(renderer, 255, 255, 255, 255);
            SDL_Rect left_highlight = {left_eye.x + 1, left_eye.y + 1, 1, 1};
            SDL_Rect right_highlight = {right_eye.x + 1, right_eye.y + 1, 1, 1};
            SDL_RenderFillRect(renderer, &left_highlight);
            SDL_RenderFillRect(renderer, &right_highlight);
            
        } else {
            // Body - darker green with gradient based on distance from head
            Uint8 body_color[] = COLOR_SNAKE_BODY;
            int fade = (i * 30) / snake->length;
            
            // Apply power-up effects to body color
            if (powerups_is_double_score_active(power_ups)) {
                body_color[1] += 50; // More green for double score
            }
            
            set_color(renderer, 
                     body_color[0] - fade, 
                     body_color[1] - fade, 
                     body_color[2] - fade, 
                     body_color[3]);
            
            // Body segments get smaller towards the tail
            int segment_size = GRID_SIZE - 3 - (i / 5);
            if (segment_size < GRID_SIZE - 6) segment_size = GRID_SIZE - 6;
            int offset = (GRID_SIZE - segment_size) / 2;
            
            SDL_Rect body_rect = {x + offset, y + offset, segment_size, segment_size};
            SDL_RenderFillRect(renderer, &body_rect);
            
            // Add subtle highlight for 3D effect
            set_color(renderer, 
                     body_color[0] + 20, 
                     body_color[1] + 20, 
                     body_color[2] + 20, 
                     100);
            SDL_Rect highlight = {x + offset + 1, y + offset + 1, segment_size/3, segment_size/3};
            SDL_RenderFillRect(renderer, &highlight);
        }
        
        // Add subtle border to make segments distinct
        Uint8 border_color[] = COLOR_BORDER;
        set_color(renderer, border_color[0], border_color[1], border_color[2], 100);
        SDL_Rect border_rect = {x, y, GRID_SIZE, GRID_SIZE};
        SDL_RenderDrawRect(renderer, &border_rect);
    }
}

void render_food(SDL_Renderer* renderer, Food* food) {
    if (!food->active) return;
    
    int x = food->x * GRID_SIZE;
    int y = food->y * GRID_SIZE;
    
    // Get food-specific color
    Uint8 r, g, b;
    get_food_color(food->type, &r, &g, &b);
    
    // Enhanced pulsing effect based on food type
    float pulse_factor = 1.0f + 0.3f * sinf(food->pulse_phase) * food->glow_intensity;
    if (food->type == FOOD_MEGA) {
        pulse_factor = 1.0f + 0.5f * sinf(food->pulse_phase * 2) * food->glow_intensity;
    }
    
    int size = (int)(GRID_SIZE * 0.8f * pulse_factor);
    int offset = (GRID_SIZE - size) / 2;
    
    // Draw enhanced glow effect based on food type
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    int glow_layers = 6;
    if (food->type == FOOD_GOLDEN) glow_layers = 8;
    if (food->type == FOOD_MEGA) glow_layers = 10;
    
    for (int glow = glow_layers; glow > 0; glow--) {
        int glow_alpha = food->type == FOOD_MEGA ? 40 : 20;
        set_color(renderer, r, g, b, glow_alpha);
        int glow_size = size + glow * 2;
        int glow_offset = (GRID_SIZE - glow_size) / 2;
        SDL_Rect glow_rect = {x + glow_offset, y + glow_offset, glow_size, glow_size};
        SDL_RenderFillRect(renderer, &glow_rect);
    }
    
    // Food main body
    set_color(renderer, r, g, b, 255);
    SDL_Rect food_rect = {x + offset, y + offset, size, size};
    SDL_RenderFillRect(renderer, &food_rect);
    
    // Add type-specific visual elements
    switch (food->type) {
        case FOOD_SPEED:
            // Lightning bolt effect
            set_color(renderer, 255, 255, 255, 200);
            for (int i = 0; i < 3; i++) {
                SDL_Rect bolt = {x + offset + i*3, y + offset + i*2, 2, size/3};
                SDL_RenderFillRect(renderer, &bolt);
            }
            break;
            
        case FOOD_DOUBLE:
            // Double circle effect
            set_color(renderer, 255, 255, 255, 150);
            SDL_Rect inner1 = {x + offset + 2, y + offset + 2, size/3, size/3};
            SDL_Rect inner2 = {x + offset + size - size/3, y + offset + size - size/3, size/3, size/3};
            SDL_RenderFillRect(renderer, &inner1);
            SDL_RenderFillRect(renderer, &inner2);
            break;
            
        case FOOD_GOLDEN:
            // Star pattern
            set_color(renderer, 255, 255, 200, 255);
            for (int i = 0; i < 4; i++) {
                SDL_Rect star_part = {x + offset + (i%2)*size/2, y + offset + (i/2)*size/2, size/4, size/4};
                SDL_RenderFillRect(renderer, &star_part);
            }
            break;
            
        case FOOD_SHRINK:
            // Spiral effect
            set_color(renderer, 255, 255, 255, 180);
            for (int i = 0; i < 6; i++) {
                float angle = food->pulse_phase + i * M_PI / 3;
                int sx = x + offset + size/2 + (int)(cosf(angle) * size/4);
                int sy = y + offset + size/2 + (int)(sinf(angle) * size/4);
                SDL_Rect spiral_dot = {sx, sy, 2, 2};
                SDL_RenderFillRect(renderer, &spiral_dot);
            }
            break;
            
        case FOOD_PHASE:
            // Phase rings
            set_color(renderer, 255, 255, 255, 100);
            for (int ring = 1; ring <= 3; ring++) {
                int ring_size = size/4 + ring * 2;
                SDL_Rect ring_rect = {x + offset + size/2 - ring_size/2, y + offset + size/2 - ring_size/2, ring_size, ring_size};
                SDL_RenderDrawRect(renderer, &ring_rect);
            }
            break;
            
        case FOOD_MEGA:
            // Rotating energy effect
            set_color(renderer, 255, 255, 255, 255);
            for (int i = 0; i < 8; i++) {
                float angle = food->pulse_phase * 2 + i * M_PI / 4;
                int ex = x + offset + size/2 + (int)(cosf(angle) * size/3);
                int ey = y + offset + size/2 + (int)(sinf(angle) * size/3);
                SDL_Rect energy_dot = {ex - 1, ey - 1, 3, 3};
                SDL_RenderFillRect(renderer, &energy_dot);
            }
            break;
            
        default:
            // Normal food highlights
            set_color(renderer, 255, 180, 200, 255);
            SDL_Rect highlight1 = {x + offset + 2, y + offset + 2, size / 3, size / 3};
            SDL_RenderFillRect(renderer, &highlight1);
            
            set_color(renderer, 255, 220, 230, 180);
            SDL_Rect highlight2 = {x + offset + size - size/4, y + offset + size - size/4, size / 6, size / 6};
            SDL_RenderFillRect(renderer, &highlight2);
            break;
    }
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, Uint8 r, Uint8 g, Uint8 b) {
    if (!font) return;
    
    SDL_Color color = {r, g, b, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            int w, h;
            SDL_QueryTexture(texture, NULL, NULL, &w, &h);
            SDL_Rect dest_rect = {x, y, w, h};
            SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

void render_ui(Game* game) {
    char score_text[50];
    char high_score_text[50];
    char level_text[50];
    char combo_text[50];
    
    sprintf(score_text, "Score: %d", game->score);
    sprintf(high_score_text, "High Score: %d", game->high_score);
    sprintf(level_text, "Level: %d", game->level);
    
    Uint8 text_color[] = COLOR_UI_TEXT;
    render_text(game->renderer, game->font, score_text, 10, 10, 
                text_color[0], text_color[1], text_color[2]);
    render_text(game->renderer, game->font, high_score_text, 10, 35, 
                text_color[0], text_color[1], text_color[2]);
    render_text(game->renderer, game->font, level_text, 10, 60,
                255, 255, 100);
    
    // Progress to next level
    int foods_for_next = level_get_required_foods(game->level);
    char progress_text[50];
    sprintf(progress_text, "Progress: %d/%d", game->foods_eaten, foods_for_next);
    render_text(game->renderer, game->font, progress_text, 10, 85,
                200, 255, 200);
    
    // Combo multiplier (if active)
    if (game->power_ups.combo_multiplier > 1) {
        sprintf(combo_text, "COMBO x%d!", game->power_ups.combo_multiplier);
        render_text(game->renderer, game->font, combo_text, 10, 110,
                    255, 150, 50);
    }
    
    // Length and special foods eaten
    char stats_text[100];
    sprintf(stats_text, "Length: %d  Special: %d", game->snake.length, game->special_foods_eaten);
    render_text(game->renderer, game->font, stats_text, 10, 135, 
                200, 200, 255);
    
    // Difficulty indicator
    const char* diff_names[] = {"Easy", "Normal", "Hard"};
    char diff_text[50];
    sprintf(diff_text, "Difficulty: %s", diff_names[game->difficulty]);
    render_text(game->renderer, game->font, diff_text, 10, 160, 
                255, 200, 100);
    
    // Game time
    if (game->state == GAME_PLAYING) {
        Uint32 game_time = (SDL_GetTicks() - game->game_start_time) / 1000;
        char time_text[50];
        sprintf(time_text, "Time: %02d:%02d", game_time / 60, game_time % 60);
        render_text(game->renderer, game->font, time_text, SCREEN_WIDTH - 120, 10, 
                    200, 255, 200);
    }
}

void render_menu(Game* game) {
    Uint8 text_color[] = COLOR_UI_TEXT;
    
    // Animated title with glow effect
    float time = SDL_GetTicks() / 1000.0f;
    float title_glow = 0.8f + 0.2f * sinf(time * 2);
    
    render_text(game->renderer, game->large_font, "SNAKE SDL2", 
                SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 120, 
                (Uint8)(text_color[0] * title_glow), 
                (Uint8)(text_color[1] * title_glow), 
                (Uint8)(text_color[2] * title_glow));
    
    // Enhanced subtitle with color cycling
    Uint8 subtitle_r = 150 + (Uint8)(50 * sinf(time));
    Uint8 subtitle_g = 200 + (Uint8)(55 * sinf(time + 2));
    Uint8 subtitle_b = 255;
    render_text(game->renderer, game->font, "Enhanced Edition with Power-Ups!", 
                SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 70, 
                subtitle_r, subtitle_g, subtitle_b);
    
    // Feature highlights
    render_text(game->renderer, game->font, "✦ Multiple Food Types ✦ Power-Up System ✦", 
                SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 45, 
                255, 200, 150);
    render_text(game->renderer, game->font, "✦ Combo System ✦ Visual Effects ✦", 
                SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 - 25, 
                200, 255, 200);
    
    // Difficulty selection with better visuals
    render_text(game->renderer, game->font, "Select Difficulty:", 
                SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 5, 
                255, 255, 150);
    
    const char* diff_names[] = {"1 - Easy (Learning)", "2 - Normal (Balanced)", "3 - Hard (Challenge)"};
    Uint8 diff_colors[][3] = {{100, 255, 100}, {255, 255, 100}, {255, 100, 100}};
    
    for (int i = 0; i < 3; i++) {
        Uint8* color = diff_colors[i];
        if ((Difficulty)i == game->difficulty) {
            // Animated highlight for selected difficulty
            float highlight_intensity = 0.7f + 0.3f * sinf(time * 4);
            render_text(game->renderer, game->font, diff_names[i], 
                        SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 30 + i * 25, 
                        (Uint8)(255 * highlight_intensity), 
                        (Uint8)(255 * highlight_intensity), 
                        (Uint8)(255 * highlight_intensity));
        } else {
            render_text(game->renderer, game->font, diff_names[i], 
                        SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 30 + i * 25, 
                        color[0], color[1], color[2]);
        }
    }
    
    // Instructions with pulsing effect
    float pulse = 0.8f + 0.2f * sinf(time * 3);
    render_text(game->renderer, game->font, "Press SPACE to start", 
                SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 130, 
                (Uint8)(text_color[0] * pulse), 
                (Uint8)(text_color[1] * pulse), 
                (Uint8)(text_color[2] * pulse));
    
    render_text(game->renderer, game->font, "Use ARROW KEYS or WASD to move", 
                SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 155, 
                200, 200, 200);
    
    render_text(game->renderer, game->font, "ESC to quit", 
                SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 180, 
                200, 200, 200);
    
    // Enhanced high score display
    if (game->high_score > 0) {
        char high_score_text[50];
        sprintf(high_score_text, "Best Score: %d", game->high_score);
        float glow = 0.9f + 0.1f * sinf(time * 1.5f);
        render_text(game->renderer, game->font, high_score_text, 
                    SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 205, 
                    (Uint8)(255 * glow), (Uint8)(255 * glow), 100);
    }
    
    // Food type legend
    render_text(game->renderer, game->font, "Food Types:", 
                SCREEN_WIDTH - 150, SCREEN_HEIGHT/2 - 100, 
                200, 200, 200);
    
    const char* food_descriptions[] = {
        "Normal - Growth",
        "Speed - Fast boost", 
        "Double - 2x score",
        "Golden - Big bonus",
        "Shrink - Smaller",
        "Phase - Walk walls",
        "MEGA - Huge bonus"
    };
    
    for (int i = 0; i < 7; i++) {
        Uint8 r, g, b;
        get_food_color(i, &r, &g, &b);
        render_text(game->renderer, game->font, food_descriptions[i], 
                    SCREEN_WIDTH - 140, SCREEN_HEIGHT/2 - 75 + i * 18, 
                    r, g, b);
    }
}

void render_game_over(Game* game) {
    // Semi-transparent overlay with gradient
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    
    // Create a more dramatic overlay
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float factor = (float)y / SCREEN_HEIGHT;
        Uint8 overlay_alpha = (Uint8)(game->game_over_alpha * (0.7f + 0.3f * factor));
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, overlay_alpha);
        SDL_RenderDrawLine(game->renderer, 0, y, SCREEN_WIDTH, y);
    }
    
    if (game->game_over_alpha < 200) return; // Wait for overlay to fade in
    
    Uint8 text_color[] = COLOR_UI_TEXT;
    
    // Animated Game Over text with pulsing effect
    float time = SDL_GetTicks() / 1000.0f;
    float pulse = 0.8f + 0.2f * sinf(time * 4);
    
    render_text(game->renderer, game->large_font, "GAME OVER", 
                SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 - 80, 
                (Uint8)(255 * pulse), 100, 100);
    
    // Final score with more prominence
    char final_score[50];
    sprintf(final_score, "Final Score: %d", game->score);
    render_text(game->renderer, game->font, final_score, 
                SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 20, 
                text_color[0], text_color[1], text_color[2]);
    
    // Game statistics
    char stats_text[100];
    sprintf(stats_text, "Length Reached: %d", game->snake.length);
    render_text(game->renderer, game->font, stats_text, 
                SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 5, 
                200, 255, 200);
    
    sprintf(stats_text, "Level Reached: %d", game->level);
    render_text(game->renderer, game->font, stats_text, 
                SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 30, 
                255, 200, 255);
    
    sprintf(stats_text, "Special Foods: %d", game->special_foods_eaten);
    render_text(game->renderer, game->font, stats_text, 
                SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 55, 
                255, 255, 150);
    
    // Game time
    Uint32 total_time = (SDL_GetTicks() - game->game_start_time) / 1000;
    sprintf(stats_text, "Time Played: %02d:%02d", total_time / 60, total_time % 60);
    render_text(game->renderer, game->font, stats_text, 
                SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 80, 
                150, 255, 255);
    
    // New high score message with celebration effect
    if (game->score == game->high_score && game->score > 0) {
        float celebration = 1.0f + 0.3f * sinf(time * 6);
        render_text(game->renderer, game->font, "🎉 NEW HIGH SCORE! 🎉", 
                    SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 110, 
                    (Uint8)(255 * celebration), (Uint8)(255 * celebration), 100);
    }
    
    // Instructions with pulsing
    float instruction_pulse = 0.7f + 0.3f * sinf(time * 2);
    render_text(game->renderer, game->font, "Press R to restart", 
                SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 + 140, 
                (Uint8)(200 * instruction_pulse), (Uint8)(200 * instruction_pulse), (Uint8)(200 * instruction_pulse));
    
    render_text(game->renderer, game->font, "Press SPACE for menu", 
                SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 165, 
                (Uint8)(200 * instruction_pulse), (Uint8)(200 * instruction_pulse), (Uint8)(200 * instruction_pulse));
}

void add_particle(Game* game, float x, float y, float vx, float vy, Uint8 r, Uint8 g, Uint8 b) {
    if (game->particle_count < MAX_PARTICLES) {
        Particle* p = &game->particles[game->particle_count];
        p->x = x;
        p->y = y;
        p->vx = vx;
        p->vy = vy;
        p->life = 1.0f;
        p->r = r;
        p->g = g;
        p->b = b;
        p->a = 255;
        game->particle_count++;
    }
}

void update_particles(Game* game) {
    for (int i = 0; i < game->particle_count; i++) {
        Particle* p = &game->particles[i];
        
        // Update position
        p->x += p->vx;
        p->y += p->vy;
        
        // Apply gravity
        p->vy += 0.2f;
        
        // Decrease life
        p->life -= 0.02f;
        p->a = (Uint8)(255 * p->life);
        
        // Remove dead particles
        if (p->life <= 0) {
            // Move last particle to current position
            game->particles[i] = game->particles[game->particle_count - 1];
            game->particle_count--;
            i--; // Check this position again
        }
    }
}

void render_particles(Game* game) {
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    
    for (int i = 0; i < game->particle_count; i++) {
        Particle* p = &game->particles[i];
        
        // Draw particle as a small rectangle
        SDL_SetRenderDrawColor(game->renderer, p->r, p->g, p->b, p->a);
        SDL_Rect particle_rect = {(int)p->x - 1, (int)p->y - 1, 3, 3};
        SDL_RenderFillRect(game->renderer, &particle_rect);
    }
}

void spawn_food_particles(Game* game, int x, int y, FoodType type) {
    // Get type-specific colors
    Uint8 r, g, b;
    get_food_color(type, &r, &g, &b);
    
    // Spawn more particles for special foods
    int particle_count = 8;
    if (type == FOOD_GOLDEN) particle_count = 12;
    if (type == FOOD_MEGA) particle_count = 16;
    
    // Spawn multiple particles in different directions
    for (int i = 0; i < particle_count; i++) {
        float angle = (i / (float)particle_count) * 2 * M_PI;
        float speed = 2.0f + (rand() % 3);
        if (type == FOOD_MEGA) speed += 2.0f; // Faster particles for mega food
        
        float vx = cosf(angle) * speed;
        float vy = sinf(angle) * speed - 1; // Start with upward velocity
        
        add_particle(game, x, y, vx, vy, r, g, b);
    }
}

void spawn_power_up_particles(Game* game, int x, int y, FoodType type) {
    if (type == FOOD_NORMAL) return; // No special particles for normal food
    
    // Create special effect particles for power-ups
    Uint8 r = 255, g = 255, b = 255; // White sparkles
    
    for (int i = 0; i < 6; i++) {
        float angle = (rand() / (float)RAND_MAX) * 2 * M_PI;
        float speed = 3.0f + (rand() % 2);
        float vx = cosf(angle) * speed;
        float vy = sinf(angle) * speed - 2; // More upward velocity
        
        add_particle(game, x + (rand() % 10 - 5), y + (rand() % 10 - 5), vx, vy, r, g, b);
    }
}

void render_power_up_indicators(Game* game) {
    int y_offset = SCREEN_HEIGHT - 40;
    
    // Speed boost indicator
    if (powerups_is_speed_active(&game->power_ups)) {
        render_text(game->renderer, game->font, "SPEED BOOST", 
                    10, y_offset, 150, 150, 255);
        y_offset -= 25;
    }
    
    // Double score indicator
    if (powerups_is_double_score_active(&game->power_ups)) {
        render_text(game->renderer, game->font, "DOUBLE SCORE", 
                    10, y_offset, 255, 200, 50);
        y_offset -= 25;
    }
    
    // Phase indicator
    if (powerups_is_phase_active(&game->power_ups)) {
        render_text(game->renderer, game->font, "PHASE MODE", 
                    10, y_offset, 255, 150, 255);
        y_offset -= 25;
    }
}

void render_gradient_background(SDL_Renderer* renderer) {
    // Create an animated starfield background
    static Uint32 last_time = 0;
    Uint32 current_time = SDL_GetTicks();
    
    // Create a dynamic gradient background
    Uint8 bg_color[] = COLOR_BACKGROUND;
    float time_factor = sinf(current_time / 5000.0f) * 0.1f; // Slow color shift
    
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float factor = (float)y / SCREEN_HEIGHT;
        Uint8 r = (Uint8)(bg_color[0] * (1.0f - factor * 0.3f + time_factor));
        Uint8 g = (Uint8)(bg_color[1] * (1.0f - factor * 0.2f + time_factor));
        Uint8 b = (Uint8)(bg_color[2] + factor * 10 + time_factor * 20);
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
    
    // Add animated stars
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    srand(42); // Fixed seed for consistent stars
    
    for (int i = 0; i < 50; i++) {
        int star_x = rand() % SCREEN_WIDTH;
        int star_y = rand() % SCREEN_HEIGHT;
        
        // Animate star brightness
        float star_time = current_time / 1000.0f + i * 0.1f;
        float brightness = 0.3f + 0.7f * sinf(star_time);
        Uint8 alpha = (Uint8)(100 * brightness);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
        SDL_Rect star = {star_x, star_y, 2, 2};
        SDL_RenderFillRect(renderer, &star);
        
        // Some stars twinkle
        if (i % 5 == 0) {
            float twinkle = 0.5f + 0.5f * sinf(star_time * 3);
            SDL_SetRenderDrawColor(renderer, 200, 200, 255, (Uint8)(50 * twinkle));
            SDL_Rect twinkle_rect = {star_x - 1, star_y - 1, 4, 4};
            SDL_RenderFillRect(renderer, &twinkle_rect);
        }
    }
    
    last_time = current_time;
}