#include "game.h"

void snake_init(Snake* snake) {
    // Start with a snake of length 3 in the center
    snake->length = 3;
    snake->direction = DIR_RIGHT;
    snake->next_direction = DIR_RIGHT;
    
    int start_x = GRID_WIDTH / 2;
    int start_y = GRID_HEIGHT / 2;
    
    snake->segments[0].x = start_x;
    snake->segments[0].y = start_y;
    snake->segments[1].x = start_x - 1;
    snake->segments[1].y = start_y;
    snake->segments[2].x = start_x - 2;
    snake->segments[2].y = start_y;
}

void snake_move(Snake* snake) {
    // Update direction
    snake->direction = snake->next_direction;
    
    // Move body segments
    for (int i = snake->length - 1; i > 0; i--) {
        snake->segments[i] = snake->segments[i - 1];
    }
    
    // Move head
    switch (snake->direction) {
        case DIR_UP:
            snake->segments[0].y--;
            break;
        case DIR_DOWN:
            snake->segments[0].y++;
            break;
        case DIR_LEFT:
            snake->segments[0].x--;
            break;
        case DIR_RIGHT:
            snake->segments[0].x++;
            break;
    }
}

bool snake_check_collision(Snake* snake) {
    Segment* head = &snake->segments[0];
    
    // Check wall collision
    if (head->x < 0 || head->x >= GRID_WIDTH || 
        head->y < 0 || head->y >= GRID_HEIGHT) {
        return true;
    }
    
    // Check self collision
    for (int i = 1; i < snake->length; i++) {
        if (head->x == snake->segments[i].x && head->y == snake->segments[i].y) {
            return true;
        }
    }
    
    return false;
}

void snake_grow(Snake* snake) {
    if (snake->length < GRID_WIDTH * GRID_HEIGHT) {
        // Add new segment at the tail position
        Segment* tail = &snake->segments[snake->length - 1];
        snake->segments[snake->length] = *tail;
        snake->length++;
    }
}

void snake_change_direction(Snake* snake, Direction new_dir) {
    // Prevent the snake from reversing into itself
    switch (new_dir) {
        case DIR_UP:
            if (snake->direction != DIR_DOWN) {
                snake->next_direction = new_dir;
            }
            break;
        case DIR_DOWN:
            if (snake->direction != DIR_UP) {
                snake->next_direction = new_dir;
            }
            break;
        case DIR_LEFT:
            if (snake->direction != DIR_RIGHT) {
                snake->next_direction = new_dir;
            }
            break;
        case DIR_RIGHT:
            if (snake->direction != DIR_LEFT) {
                snake->next_direction = new_dir;
            }
            break;
    }
}