#include "game.h"

FoodType food_get_random_type(int level, int foods_eaten) {
    int rand_val = rand() % 100;
    
    // Base probabilities
    if (rand_val < 60) return FOOD_NORMAL;
    if (rand_val < 75) return FOOD_SPEED;
    if (rand_val < 85) return FOOD_DOUBLE;
    if (rand_val < 92) return FOOD_GOLDEN;
    if (rand_val < 97) return FOOD_SHRINK;
    if (rand_val < 99) return FOOD_PHASE;
    
    // MEGA food becomes more likely at higher levels
    if (level > 5 || foods_eaten > 20) {
        return FOOD_MEGA;
    }
    
    return FOOD_GOLDEN; // Fallback
}

void get_food_color(FoodType type, Uint8* r, Uint8* g, Uint8* b) {
    switch (type) {
        case FOOD_NORMAL:
            *r = 255; *g = 80; *b = 120; // Pink
            break;
        case FOOD_SPEED:
            *r = 120; *g = 200; *b = 255; // Light blue
            break;
        case FOOD_DOUBLE:
            *r = 255; *g = 200; *b = 80; // Orange
            break;
        case FOOD_GOLDEN:
            *r = 255; *g = 215; *b = 0; // Gold
            break;
        case FOOD_SHRINK:
            *r = 200; *g = 100; *b = 200; // Purple
            break;
        case FOOD_PHASE:
            *r = 100; *g = 255; *b = 100; // Green
            break;
        case FOOD_MEGA:
            *r = 255; *g = 100; *b = 255; // Magenta
            break;
        default:
            *r = 255; *g = 80; *b = 120;
            break;
    }
}

void food_spawn(Food* food, Snake* snake) {
    bool valid_position = false;
    
    while (!valid_position) {
        food->x = rand() % GRID_WIDTH;
        food->y = rand() % GRID_HEIGHT;
        
        valid_position = true;
        
        // Make sure food doesn't spawn on snake
        for (int i = 0; i < snake->length; i++) {
            if (food->x == snake->segments[i].x && food->y == snake->segments[i].y) {
                valid_position = false;
                break;
            }
        }
    }
    
    food->active = true;
    food->spawn_time = SDL_GetTicks() / 1000.0f;
    food->pulse_phase = 0.0f;
    food->glow_intensity = 1.0f;
}

bool food_check_collision(Food* food, Snake* snake) {
    if (!food->active) {
        return false;
    }
    
    Segment* head = &snake->segments[0];
    if (head->x == food->x && head->y == food->y) {
        food->active = false;
        return true;
    }
    
    return false;
}

void food_apply_effect(Game* game, FoodType type) {
    Uint32 current_time = SDL_GetTicks();
    
    switch (type) {
        case FOOD_NORMAL:
            snake_grow(&game->snake);
            game->score += game->base_score_per_food * powerups_get_score_multiplier(&game->power_ups);
            break;
            
        case FOOD_SPEED:
            snake_grow(&game->snake);
            game->power_ups.speed_boost = true;
            game->power_ups.speed_end_time = current_time + 3000; // 3 seconds
            game->score += game->base_score_per_food * 2 * powerups_get_score_multiplier(&game->power_ups);
            break;
            
        case FOOD_DOUBLE:
            snake_grow(&game->snake);
            game->power_ups.double_score = true;
            game->power_ups.double_score_end_time = current_time + 5000; // 5 seconds
            game->score += game->base_score_per_food * powerups_get_score_multiplier(&game->power_ups);
            break;
            
        case FOOD_GOLDEN:
            snake_grow(&game->snake);
            game->score += game->base_score_per_food * 5 * powerups_get_score_multiplier(&game->power_ups);
            break;
            
        case FOOD_SHRINK:
            // Shrink snake by 2 segments if possible
            if (game->snake.length > 5) {
                game->snake.length -= 2;
            } else if (game->snake.length > 3) {
                game->snake.length--;
            }
            game->score += game->base_score_per_food * 3 * powerups_get_score_multiplier(&game->power_ups);
            break;
            
        case FOOD_PHASE:
            snake_grow(&game->snake);
            game->power_ups.phase_through_walls = true;
            game->power_ups.phase_end_time = current_time + 4000; // 4 seconds
            game->score += game->base_score_per_food * 3 * powerups_get_score_multiplier(&game->power_ups);
            break;
            
        case FOOD_MEGA:
            snake_grow(&game->snake);
            game->score += game->base_score_per_food * 10 * powerups_get_score_multiplier(&game->power_ups);
            // Add screen shake for mega food
            add_screen_shake(game, 8.0f, 500);
            break;
    }
    
    // Update combo system
    if (current_time - game->power_ups.last_food_time < 1500) { // Within 1.5 seconds
        game->power_ups.combo_count++;
        if (game->power_ups.combo_count > 1) {
            game->power_ups.combo_multiplier = (game->power_ups.combo_count / 2) + 1;
            if (game->power_ups.combo_multiplier > 5) game->power_ups.combo_multiplier = 5;
        }
    } else {
        game->power_ups.combo_count = 1;
        game->power_ups.combo_multiplier = 1;
    }
    game->power_ups.last_food_time = current_time;
    
    game->foods_eaten++;
    if (type != FOOD_NORMAL) {
        game->special_foods_eaten++;
    }
}