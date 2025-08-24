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

// Food types with special effects
typedef enum {
    FOOD_NORMAL,      // Regular food, grows snake
    FOOD_SPEED,       // Temporary speed boost
    FOOD_DOUBLE,      // Double score for next few foods
    FOOD_GOLDEN,      // High score bonus
    FOOD_SHRINK,      // Shrinks snake by 1-2 segments
    FOOD_PHASE,       // Temporary wall phasing ability
    FOOD_MEGA,        // Massive score bonus, very rare
    FOOD_TYPE_COUNT
} FoodType;

// Power-up states
typedef struct {
    bool speed_boost;
    bool double_score;
    bool phase_through_walls;
    Uint32 speed_end_time;
    Uint32 double_score_end_time;
    Uint32 phase_end_time;
    int combo_multiplier;
    int combo_count;
    Uint32 last_food_time;
} PowerUps;

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
    FoodType type;
    float spawn_time;
    float pulse_phase;
    float glow_intensity;
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
    
    // Power-up system
    PowerUps power_ups;
    
    // Level system
    int level;
    int foods_needed_for_level;
    int base_score_per_food;
    
    // Enhanced statistics
    int foods_eaten;
    int special_foods_eaten;
    Uint32 game_start_time;
    
    // Screen effects
    float screen_shake_intensity;
    Uint32 screen_shake_end_time;
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
FoodType food_get_random_type(int level, int foods_eaten);
void food_apply_effect(Game* game, FoodType type);

// Power-up functions
void powerups_init(PowerUps* power_ups);
void powerups_update(PowerUps* power_ups, Uint32 current_time);
bool powerups_is_speed_active(PowerUps* power_ups);
bool powerups_is_double_score_active(PowerUps* power_ups);
bool powerups_is_phase_active(PowerUps* power_ups);
int powerups_get_score_multiplier(PowerUps* power_ups);

// Level functions
void level_up(Game* game);
int level_get_required_foods(int level);
Uint32 level_get_speed(int level, bool speed_boost);

// Rendering functions
void render_snake(SDL_Renderer* renderer, Snake* snake, PowerUps* power_ups);
void render_food(SDL_Renderer* renderer, Food* food);
void render_ui(Game* game);
void render_menu(Game* game);
void render_game_over(Game* game);
void render_particles(Game* game);
void render_power_up_indicators(Game* game);

// Particle functions
void add_particle(Game* game, float x, float y, float vx, float vy, Uint8 r, Uint8 g, Uint8 b);
void update_particles(Game* game);
void spawn_food_particles(Game* game, int x, int y, FoodType type);
void spawn_power_up_particles(Game* game, int x, int y, FoodType type);

// Utility functions
void set_color(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void draw_rounded_rect(SDL_Renderer* renderer, int x, int y, int w, int h, int radius);
void render_gradient_background(SDL_Renderer* renderer);
void add_screen_shake(Game* game, float intensity, Uint32 duration);
void get_food_color(FoodType type, Uint8* r, Uint8* g, Uint8* b);

// Save/Load system
void save_high_score(int score);
int load_high_score(void);

#endif