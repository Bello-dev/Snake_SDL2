#include "game.h"

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