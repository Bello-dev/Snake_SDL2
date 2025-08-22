#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Screen dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define GRID_SIZE 20
#define GRID_WIDTH (SCREEN_WIDTH / GRID_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / GRID_SIZE)

// Colors (RGBA) - Enhanced color scheme
#define COLOR_BACKGROUND {12, 16, 40, 255}
#define COLOR_SNAKE_HEAD {100, 255, 150, 255}
#define COLOR_SNAKE_BODY {60, 200, 120, 255}
#define COLOR_FOOD {255, 80, 120, 255}
#define COLOR_UI_TEXT {240, 240, 255, 255}
#define COLOR_BORDER {80, 100, 180, 255}
#define COLOR_PARTICLE {255, 200, 100, 255}

// Game states
typedef enum {
    GAME_MENU,
    GAME_PLAYING,
    GAME_PAUSED,
    GAME_OVER
} GameState;

// Difficulty levels
typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD
} Difficulty;

// Directions
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

// Particle for visual effects
typedef struct {
    float x, y;
    float vx, vy;
    float life;
    Uint8 r, g, b, a;
} Particle;

#define MAX_PARTICLES 50

// Snake segment
typedef struct {
    int x, y;
} Segment;

// Snake structure
typedef struct {
    Segment segments[GRID_WIDTH * GRID_HEIGHT];
    int length;
    Direction direction;
    Direction next_direction;
} Snake;

// Food structure
typedef struct {
    int x, y;
    bool active;
} Food;

// Game structure
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* large_font;
    Mix_Chunk* eat_sound;
    Mix_Chunk* game_over_sound;
    
    Snake snake;
    Food food;
    GameState state;
    Difficulty difficulty;
    int score;
    int high_score;
    bool running;
    
    // Timing
    Uint32 last_move_time;
    Uint32 move_delay;
    Uint32 base_move_delay;
    
    // Visual effects
    float food_pulse;
    int game_over_alpha;
    Particle particles[MAX_PARTICLES];
    int particle_count;
    
    // Statistics
    int foods_eaten;
    Uint32 game_start_time;
} Game;

// Function declarations
bool game_init(Game* game);
void game_cleanup(Game* game);
void game_handle_events(Game* game);
void game_update(Game* game);
void game_render(Game* game);
void game_reset(Game* game);

// Snake functions
void snake_init(Snake* snake);
void snake_move(Snake* snake);
bool snake_check_collision(Snake* snake);
void snake_grow(Snake* snake);
void snake_change_direction(Snake* snake, Direction new_dir);

// Food functions
void food_spawn(Food* food, Snake* snake);
bool food_check_collision(Food* food, Snake* snake);

// Rendering functions
void render_snake(SDL_Renderer* renderer, Snake* snake);
void render_food(SDL_Renderer* renderer, Food* food, float pulse);
void render_ui(Game* game);
void render_menu(Game* game);
void render_game_over(Game* game);
void render_particles(Game* game);

// Particle functions
void add_particle(Game* game, float x, float y, float vx, float vy, Uint8 r, Uint8 g, Uint8 b);
void update_particles(Game* game);
void spawn_food_particles(Game* game, int x, int y);

// Utility functions
void set_color(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void draw_rounded_rect(SDL_Renderer* renderer, int x, int y, int w, int h, int radius);
void render_gradient_background(SDL_Renderer* renderer);

#endif