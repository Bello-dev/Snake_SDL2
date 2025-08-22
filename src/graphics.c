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

void render_snake(SDL_Renderer* renderer, Snake* snake) {
    for (int i = 0; i < snake->length; i++) {
        int x = snake->segments[i].x * GRID_SIZE;
        int y = snake->segments[i].y * GRID_SIZE;
        
        if (i == 0) {
            // Head - bright green with gradient effect and glow
            Uint8 head_color[] = COLOR_SNAKE_HEAD;
            
            // Draw glow effect around head
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            for (int glow = 4; glow > 0; glow--) {
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

void render_food(SDL_Renderer* renderer, Food* food, float pulse) {
    if (!food->active) return;
    
    int x = food->x * GRID_SIZE;
    int y = food->y * GRID_SIZE;
    
    // Pulsing effect
    float pulse_factor = 1.0f + 0.3f * sin(pulse);
    int size = (int)(GRID_SIZE * 0.8f * pulse_factor);
    int offset = (GRID_SIZE - size) / 2;
    
    // Draw glow effect around food
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int glow = 6; glow > 0; glow--) {
        Uint8 food_color[] = COLOR_FOOD;
        set_color(renderer, food_color[0], food_color[1], food_color[2], 20);
        int glow_size = size + glow * 2;
        int glow_offset = (GRID_SIZE - glow_size) / 2;
        SDL_Rect glow_rect = {x + glow_offset, y + glow_offset, glow_size, glow_size};
        SDL_RenderFillRect(renderer, &glow_rect);
    }
    
    // Food - bright red with pulsing effect
    Uint8 food_color[] = COLOR_FOOD;
    set_color(renderer, food_color[0], food_color[1], food_color[2], food_color[3]);
    
    SDL_Rect food_rect = {x + offset, y + offset, size, size};
    SDL_RenderFillRect(renderer, &food_rect);
    
    // Add multiple highlights for more appealing look
    set_color(renderer, 255, 180, 200, 255);
    SDL_Rect highlight1 = {x + offset + 2, y + offset + 2, size / 3, size / 3};
    SDL_RenderFillRect(renderer, &highlight1);
    
    set_color(renderer, 255, 220, 230, 180);
    SDL_Rect highlight2 = {x + offset + size - size/4, y + offset + size - size/4, size / 6, size / 6};
    SDL_RenderFillRect(renderer, &highlight2);
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
    char stats_text[100];
    
    sprintf(score_text, "Score: %d", game->score);
    sprintf(high_score_text, "High Score: %d", game->high_score);
    
    Uint8 text_color[] = COLOR_UI_TEXT;
    render_text(game->renderer, game->font, score_text, 10, 10, 
                text_color[0], text_color[1], text_color[2]);
    render_text(game->renderer, game->font, high_score_text, 10, 40, 
                text_color[0], text_color[1], text_color[2]);
    
    // Speed indicator
    char speed_text[50];
    int speed_level = (game->base_move_delay - game->move_delay) / 5 + 1;
    sprintf(speed_text, "Speed: %d", speed_level);
    render_text(game->renderer, game->font, speed_text, 10, 70, 
                text_color[0], text_color[1], text_color[2]);
    
    // Length and food eaten
    sprintf(stats_text, "Length: %d  Foods: %d", game->snake.length, game->foods_eaten);
    render_text(game->renderer, game->font, stats_text, 10, 100, 
                200, 200, 255);
    
    // Difficulty indicator
    const char* diff_names[] = {"Easy", "Normal", "Hard"};
    char diff_text[50];
    sprintf(diff_text, "Difficulty: %s", diff_names[game->difficulty]);
    render_text(game->renderer, game->font, diff_text, 10, 130, 
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
    
    // Title with glow effect
    render_text(game->renderer, game->large_font, "SNAKE SDL2", 
                SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 120, 
                text_color[0], text_color[1], text_color[2]);
    
    // Subtitle
    render_text(game->renderer, game->font, "Enhanced Edition", 
                SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 70, 
                150, 200, 255);
    
    // Difficulty selection
    render_text(game->renderer, game->font, "Select Difficulty:", 
                SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 30, 
                255, 255, 150);
    
    const char* diff_names[] = {"1 - Easy", "2 - Normal", "3 - Hard"};
    Uint8 diff_colors[][3] = {{100, 255, 100}, {255, 255, 100}, {255, 100, 100}};
    
    for (int i = 0; i < 3; i++) {
        Uint8* color = diff_colors[i];
        if ((Difficulty)i == game->difficulty) {
            // Highlight selected difficulty
            render_text(game->renderer, game->font, diff_names[i], 
                        SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 + 10 + i * 25, 
                        255, 255, 255);
        } else {
            render_text(game->renderer, game->font, diff_names[i], 
                        SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 + 10 + i * 25, 
                        color[0], color[1], color[2]);
        }
    }
    
    // Instructions
    render_text(game->renderer, game->font, "Press SPACE to start", 
                SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 110, 
                text_color[0], text_color[1], text_color[2]);
    
    render_text(game->renderer, game->font, "Use ARROW KEYS or WASD to move", 
                SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 140, 
                200, 200, 200);
    
    render_text(game->renderer, game->font, "ESC to quit", 
                SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 170, 
                200, 200, 200);
    
    if (game->high_score > 0) {
        char high_score_text[50];
        sprintf(high_score_text, "Best Score: %d", game->high_score);
        render_text(game->renderer, game->font, high_score_text, 
                    SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 200, 
                    255, 255, 100);
    }
}

void render_game_over(Game* game) {
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, game->game_over_alpha);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderFillRect(game->renderer, &overlay);
    
    if (game->game_over_alpha < 200) return; // Wait for overlay to fade in
    
    Uint8 text_color[] = COLOR_UI_TEXT;
    
    // Game Over text
    render_text(game->renderer, game->large_font, "GAME OVER", 
                SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 - 60, 
                255, 100, 100);
    
    // Final score
    char final_score[50];
    sprintf(final_score, "Final Score: %d", game->score);
    render_text(game->renderer, game->font, final_score, 
                SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2, 
                text_color[0], text_color[1], text_color[2]);
    
    // New high score message
    if (game->score == game->high_score && game->score > 0) {
        render_text(game->renderer, game->font, "NEW HIGH SCORE!", 
                    SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 + 30, 
                    255, 255, 100);
    }
    
    // Instructions
    render_text(game->renderer, game->font, "Press R to restart", 
                SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 + 70, 
                200, 200, 200);
    
    render_text(game->renderer, game->font, "Press SPACE for menu", 
                SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 100, 
                200, 200, 200);
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

void spawn_food_particles(Game* game, int x, int y) {
    // Spawn multiple particles in different directions
    for (int i = 0; i < 8; i++) {
        float angle = (i / 8.0f) * 2 * M_PI;
        float speed = 2.0f + (rand() % 3);
        float vx = cos(angle) * speed;
        float vy = sin(angle) * speed - 1; // Start with upward velocity
        
        Uint8 particle_color[] = COLOR_PARTICLE;
        add_particle(game, x, y, vx, vy, 
                    particle_color[0], particle_color[1], particle_color[2]);
    }
}

void render_gradient_background(SDL_Renderer* renderer) {
    // Create a simple vertical gradient from dark blue to darker blue
    Uint8 bg_color[] = COLOR_BACKGROUND;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float factor = (float)y / SCREEN_HEIGHT;
        Uint8 r = (Uint8)(bg_color[0] * (1.0f - factor * 0.3f));
        Uint8 g = (Uint8)(bg_color[1] * (1.0f - factor * 0.2f));
        Uint8 b = (Uint8)(bg_color[2] + factor * 10);
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
}