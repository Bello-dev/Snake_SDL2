#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <vector>
#include <memory>

// Forward declarations
struct GameStats;
class AchievementSystem;

// Configuration constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 20;
const int GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;
const int MAX_PARTICLES = 100;

// Game states
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER
};

// Difficulty levels
enum Difficulty {
    DIFFICULTY_EASY = 1,
    DIFFICULTY_NORMAL = 2,
    DIFFICULTY_HARD = 3
};

// Direction enum
enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

// Food types for magical power system
enum FoodType {
    FOOD_NORMAL,      // Regular food, grows snake
    FOOD_SPEED,       // Temporary speed boost
    FOOD_DOUBLE,      // Double score for next few foods
    FOOD_GOLDEN,      // High score bonus
    FOOD_SHRINK,      // Shrinks snake by 1-2 segments
    FOOD_PHASE,       // Temporary wall phasing ability
    FOOD_MEGA,        // Massive score bonus, very rare
    FOOD_TYPE_COUNT
};

// Snake segment
struct Segment {
    int x, y;
};

// Snake structure
struct Snake {
    std::vector<Segment> segments;
    Direction direction;
    Direction next_direction;
    
    Snake();
    ~Snake() = default;
    void init();
    void move();
    bool check_collision(bool phase_mode = false);
    void grow();
    void shrink(int amount = 1);
    void change_direction(Direction new_dir);
    int get_length() const { return segments.size(); }
};

// Food structure
struct Food {
    int x, y;
    bool active;
    FoodType type;
    float spawn_time;
    float pulse_phase;
    float glow_intensity;
    
    Food();
    void spawn(const Snake& snake);
    bool check_collision(const Snake& snake);
    FoodType get_random_type(int level, int foods_eaten);
    SDL_Color get_color() const;
};

// Power-up states
struct PowerUps {
    bool speed_boost;
    bool double_score;
    bool phase_through_walls;
    Uint32 speed_end_time;
    Uint32 double_score_end_time;
    Uint32 phase_end_time;
    int combo_multiplier;
    int combo_count;
    Uint32 last_food_time;
    
    PowerUps();
    void init();
    void update(Uint32 current_time);
    bool is_speed_active() const { return speed_boost; }
    bool is_double_score_active() const { return double_score; }
    bool is_phase_active() const { return phase_through_walls; }
    int get_score_multiplier() const;
};

// Particle for visual effects
struct Particle {
    float x, y;
    float vx, vy;
    float life;
    float max_life;
    Uint8 r, g, b, a;
    
    Particle() : x(0), y(0), vx(0), vy(0), life(0), max_life(1.0f), r(255), g(255), b(255), a(255) {}
    void update(float dt);
    bool is_alive() const { return life > 0; }
};

// Main Game class
class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* large_font;
    
    // Audio
    Mix_Music* background_music;
    Mix_Music* game_over_music;
    Mix_Chunk* eat_sound;
    Mix_Chunk* move_sound;
    Mix_Chunk* power_up_sound;
    Mix_Chunk* level_up_sound;
    std::vector<Mix_Chunk*> sound_effects;
    
    // Textures
    SDL_Texture* bg_texture;
    SDL_Texture* loading_texture;
    std::vector<SDL_Texture*> score_textures;
    std::vector<SDL_Texture*> letter_textures;
    
    // Game objects
    Snake snake;
    Food food;
    PowerUps power_ups;
    std::vector<Particle> particles;
    
    // Achievement system
    GameStats* game_stats;
    AchievementSystem* achievement_system;
    
    // Game state
    GameState state;
    Difficulty difficulty;
    int score;
    int high_score;
    bool running;
    
    // Level system
    int level;
    int foods_needed_for_level;
    int base_score_per_food;
    int foods_eaten;
    int special_foods_eaten;
    Uint32 game_start_time;
    
    // Timing
    Uint32 last_move_time;
    Uint32 move_delay;
    Uint32 base_move_delay;
    
    // Visual effects
    float food_pulse;
    int game_over_alpha;
    float screen_shake_intensity;
    Uint32 screen_shake_end_time;
    
public:
    Game();
    ~Game();
    
    bool init();
    void cleanup();
    void run();
    void handle_events();
    void update();
    void render();
    void reset();
    
    // Audio management
    bool load_audio();
    void play_sound(int sound_id);
    
    // Texture management  
    bool load_textures();
    SDL_Texture* load_texture(const std::string& path);
    
    // Game logic
    void apply_food_effect(FoodType type);
    void level_up();
    void add_screen_shake(float intensity, Uint32 duration);
    
    // Rendering methods
    void render_menu();
    void render_game();
    void render_game_over();
    void render_loading_screen();
    void render_snake();
    void render_food();
    void render_ui();
    void render_particles();
    void render_power_up_indicators();
    void render_text(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font_to_use = nullptr);
    void render_gradient_background();
    
    // Particle system
    void add_particle(float x, float y, float vx, float vy, SDL_Color color);
    void spawn_food_particles(float x, float y, FoodType type);
    void spawn_power_up_particles(float x, float y, FoodType type);
    void update_particles();
    
    // Utility functions
    Uint32 get_level_speed(int level, bool speed_boost);
    int get_level_required_foods(int level);
    void save_high_score();
    void load_high_score();
};

#endif // GAME_H