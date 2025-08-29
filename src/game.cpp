#include "game.h"
#include "achievements.h"
#include <string>
#include <fstream>
#include <algorithm>

// Snake implementation
Snake::Snake() {
    init();
}

void Snake::init() {
    segments.clear();
    direction = DIR_RIGHT;
    next_direction = DIR_RIGHT;
    
    int start_x = GRID_WIDTH / 2;
    int start_y = GRID_HEIGHT / 2;
    
    segments.push_back({start_x, start_y});
    segments.push_back({start_x - 1, start_y});
    segments.push_back({start_x - 2, start_y});
}

void Snake::move() {
    direction = next_direction;
    
    // Move body segments
    for (int i = segments.size() - 1; i > 0; i--) {
        segments[i] = segments[i - 1];
    }
    
    // Move head
    switch (direction) {
        case DIR_UP:
            segments[0].y--;
            break;
        case DIR_DOWN:
            segments[0].y++;
            break;
        case DIR_LEFT:
            segments[0].x--;
            break;
        case DIR_RIGHT:
            segments[0].x++;
            break;
    }
}

bool Snake::check_collision(bool phase_mode) {
    const Segment& head = segments[0];
    
    // Check wall collision (unless in phase mode)
    if (!phase_mode) {
        if (head.x < 0 || head.x >= GRID_WIDTH || 
            head.y < 0 || head.y >= GRID_HEIGHT) {
            return true;
        }
    } else {
        // In phase mode, wrap around walls
        if (head.x < 0 || head.x >= GRID_WIDTH || 
            head.y < 0 || head.y >= GRID_HEIGHT) {
            segments[0].x = (head.x + GRID_WIDTH) % GRID_WIDTH;
            segments[0].y = (head.y + GRID_HEIGHT) % GRID_HEIGHT;
        }
    }
    
    // Check self collision
    for (size_t i = 1; i < segments.size(); i++) {
        if (head.x == segments[i].x && head.y == segments[i].y) {
            return true;
        }
    }
    
    return false;
}

void Snake::grow() {
    if (segments.size() < GRID_WIDTH * GRID_HEIGHT) {
        segments.push_back(segments.back());
    }
}

void Snake::shrink(int amount) {
    for (int i = 0; i < amount && segments.size() > 3; i++) {
        segments.pop_back();
    }
}

void Snake::change_direction(Direction new_dir) {
    // Prevent the snake from reversing into itself
    switch (new_dir) {
        case DIR_UP:
            if (direction != DIR_DOWN) {
                next_direction = new_dir;
            }
            break;
        case DIR_DOWN:
            if (direction != DIR_UP) {
                next_direction = new_dir;
            }
            break;
        case DIR_LEFT:
            if (direction != DIR_RIGHT) {
                next_direction = new_dir;
            }
            break;
        case DIR_RIGHT:
            if (direction != DIR_LEFT) {
                next_direction = new_dir;
            }
            break;
    }
}

// Food implementation
Food::Food() {
    active = false;
    type = FOOD_NORMAL;
    spawn_time = 0;
    pulse_phase = 0;
    glow_intensity = 1.0f;
}

void Food::spawn(const Snake& snake) {
    do {
        x = rand() % GRID_WIDTH;
        y = rand() % GRID_HEIGHT;
    } while (std::any_of(snake.segments.begin(), snake.segments.end(),
                        [this](const Segment& seg) { return seg.x == x && seg.y == y; }));
    
    active = true;
    spawn_time = SDL_GetTicks() / 1000.0f;
    pulse_phase = 0;
    glow_intensity = 1.0f;
}

bool Food::check_collision(const Snake& snake) {
    if (!active) return false;
    return snake.segments[0].x == x && snake.segments[0].y == y;
}

FoodType Food::get_random_type(int level, int foods_eaten) {
    int random = rand() % 100;
    
    // Base probabilities that change with level
    if (random < 50) return FOOD_NORMAL;      // 50% normal food
    if (random < 65) return FOOD_SPEED;       // 15% speed boost
    if (random < 75) return FOOD_DOUBLE;      // 10% double score
    if (random < 85) return FOOD_GOLDEN;      // 10% golden food
    if (random < 92) return FOOD_SHRINK;      // 7% shrink food
    if (random < 98) return FOOD_PHASE;       // 6% phase food
    return FOOD_MEGA;                         // 2% mega food
}

SDL_Color Food::get_color() const {
    switch (type) {
        case FOOD_NORMAL: return {0, 255, 0, 255};      // Green
        case FOOD_SPEED: return {255, 255, 0, 255};     // Yellow
        case FOOD_DOUBLE: return {0, 255, 255, 255};    // Cyan
        case FOOD_GOLDEN: return {255, 215, 0, 255};    // Gold
        case FOOD_SHRINK: return {255, 0, 255, 255};    // Magenta
        case FOOD_PHASE: return {128, 0, 255, 255};     // Purple
        case FOOD_MEGA: return {255, 100, 100, 255};    // Red
        default: return {255, 255, 255, 255};           // White
    }
}

// PowerUps implementation
PowerUps::PowerUps() {
    init();
}

void PowerUps::init() {
    speed_boost = false;
    double_score = false;
    phase_through_walls = false;
    speed_end_time = 0;
    double_score_end_time = 0;
    phase_end_time = 0;
    combo_multiplier = 1;
    combo_count = 0;
    last_food_time = 0;
}

void PowerUps::update(Uint32 current_time) {
    if (speed_boost && current_time >= speed_end_time) {
        speed_boost = false;
    }
    
    if (double_score && current_time >= double_score_end_time) {
        double_score = false;
    }
    
    if (phase_through_walls && current_time >= phase_end_time) {
        phase_through_walls = false;
    }
    
    // Update combo system
    if (current_time - last_food_time > 3000) { // 3 seconds timeout
        combo_count = 0;
        combo_multiplier = 1;
    }
}

int PowerUps::get_score_multiplier() const {
    int multiplier = combo_multiplier;
    if (double_score) {
        multiplier *= 2;
    }
    return multiplier;
}

// Particle implementation
void Particle::update(float dt) {
    x += vx * dt;
    y += vy * dt;
    life -= dt;
    
    // Fade out as life decreases
    float life_ratio = life / max_life;
    a = static_cast<Uint8>(255 * std::max(0.0f, life_ratio));
}

// Game implementation
Game::Game() : window(nullptr), renderer(nullptr), font(nullptr), large_font(nullptr),
               background_music(nullptr), game_over_music(nullptr), eat_sound(nullptr),
               move_sound(nullptr), power_up_sound(nullptr), level_up_sound(nullptr),
               bg_texture(nullptr), loading_texture(nullptr),
               state(STATE_MENU), difficulty(DIFFICULTY_NORMAL), score(0), high_score(0),
               running(true), level(1), foods_needed_for_level(5), base_score_per_food(10),
               foods_eaten(0), special_foods_eaten(0), game_start_time(0),
               last_move_time(0), move_delay(200), base_move_delay(200),
               food_pulse(0), game_over_alpha(0), screen_shake_intensity(0),
               screen_shake_end_time(0), game_stats(nullptr), achievement_system(nullptr) {
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Initialize achievement system
    game_stats = new GameStats();
    achievement_system = new AchievementSystem();
}

Game::~Game() {
    cleanup();
    delete game_stats;
    delete achievement_system;
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "Erreur SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow("Snake SDL2 - Édition Révolutionnaire",
                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             SCREEN_WIDTH + 200, SCREEN_HEIGHT,
                             SDL_WINDOW_SHOWN);
    
    if (!window) {
        std::cerr << "Erreur création fenêtre: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (!renderer) {
        std::cerr << "Erreur création renderer: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "Erreur TTF_Init: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Load fonts
    font = TTF_OpenFont("data/polices/arial.ttf", 16);
    if (!font) {
        // Try system font
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
        if (!font) {
            std::cerr << "Erreur chargement police: " << TTF_GetError() << std::endl;
            return false;
        }
    }
    
    large_font = TTF_OpenFont("data/polices/arial.ttf", 24);
    if (!large_font) {
        large_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24);
        if (!large_font) {
            large_font = font; // Fallback to regular font
        }
    }
    
    // Initialize audio
    if (!load_audio()) {
        std::cerr << "Warning: Could not load audio" << std::endl;
    }
    
    // Load textures
    if (!load_textures()) {
        std::cerr << "Warning: Could not load all textures" << std::endl;
    }
    
    // Initialize particles
    particles.reserve(MAX_PARTICLES);
    
    // Load high score
    load_high_score();
    
    return true;
}

bool Game::load_audio() {
    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3) {
        std::cerr << "Warning: Audio initialization failed: " << Mix_GetError() << std::endl;
        return false; // Continue without audio
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Warning: Could not open audio device: " << Mix_GetError() << std::endl;
        return false; // Continue without audio
    }
    
    // Load background music
    background_music = Mix_LoadMUS("data/sons/music.mp3");
    if (!background_music) {
        std::cerr << "Warning: Could not load background music - continuing without music" << std::endl;
    }
    
    game_over_music = Mix_LoadMUS("data/sons/choc.mp3");
    if (!game_over_music) {
        std::cerr << "Warning: Could not load game over music - continuing without music" << std::endl;
    }
    
    // Load sound effects from the existing directory structure
    sound_effects.resize(30, nullptr);
    for (int i = 0; i < 30; i++) {
        std::string path = "data/sons/Sound Effects/sound_" + std::to_string(i) + ".wav";
        sound_effects[i] = Mix_LoadWAV(path.c_str());
        if (!sound_effects[i]) {
            // Try alternative path
            path = "data/sons/Sound/sound_" + std::to_string(i) + ".wav";
            sound_effects[i] = Mix_LoadWAV(path.c_str());
            // If still fails, just continue - sound effects are optional
        }
    }
    
    return true;
}

bool Game::load_textures() {
    // Load background - use fallback if not available
    bg_texture = load_texture("data/images/bg.jpg");
    
    // Load loading screen - use fallback if not available
    loading_texture = load_texture("data/images/chargement.jpg");
    
    // Score and letter textures are optional - the game will render text instead
    score_textures.resize(10, nullptr);
    letter_textures.resize(26, nullptr);
    
    return true; // Always return true since textures are optional
}

SDL_Texture* Game::load_texture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        // Silently continue without texture - fallbacks will be used
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        std::cerr << "Warning: Could not create texture from " << path << ": " << SDL_GetError() << std::endl;
    }
    
    return texture;
}

void Game::cleanup() {
    // Clean up textures
    if (bg_texture) SDL_DestroyTexture(bg_texture);
    if (loading_texture) SDL_DestroyTexture(loading_texture);
    
    for (auto texture : score_textures) {
        if (texture) SDL_DestroyTexture(texture);
    }
    
    for (auto texture : letter_textures) {
        if (texture) SDL_DestroyTexture(texture);
    }
    
    // Clean up audio
    if (background_music) Mix_FreeMusic(background_music);
    if (game_over_music) Mix_FreeMusic(game_over_music);
    if (eat_sound) Mix_FreeChunk(eat_sound);
    if (move_sound) Mix_FreeChunk(move_sound);
    if (power_up_sound) Mix_FreeChunk(power_up_sound);
    if (level_up_sound) Mix_FreeChunk(level_up_sound);
    
    for (auto sound : sound_effects) {
        if (sound) Mix_FreeChunk(sound);
    }
    
    // Clean up fonts
    if (font) TTF_CloseFont(font);
    if (large_font) TTF_CloseFont(large_font);
    
    // Clean up SDL
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}

void Game::run() {
    // Show loading screen first
    render_loading_screen();
    SDL_Delay(2000); // Show "BELLO DEV" for 2 seconds
    
    if (background_music) {
        Mix_PlayMusic(background_music, -1);
    }
    
    while (running) {
        handle_events();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void Game::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        
        if (event.type == SDL_KEYDOWN) {
            switch (state) {
                case STATE_MENU:
                    switch (event.key.keysym.sym) {
                        case SDLK_1:
                            difficulty = DIFFICULTY_EASY;
                            base_move_delay = 250;
                            break;
                        case SDLK_2:
                            difficulty = DIFFICULTY_NORMAL;
                            base_move_delay = 200;
                            break;
                        case SDLK_3:
                            difficulty = DIFFICULTY_HARD;
                            base_move_delay = 150;
                            break;
                        case SDLK_SPACE:
                        case SDLK_RETURN:
                            reset();
                            state = STATE_PLAYING;
                            break;
                    }
                    break;
                    
                case STATE_PLAYING:
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                        case SDLK_w:
                            snake.change_direction(DIR_UP);
                            if (sound_effects[4]) Mix_PlayChannel(1, sound_effects[4], 0);
                            break;
                        case SDLK_DOWN:
                        case SDLK_s:
                            snake.change_direction(DIR_DOWN);
                            if (sound_effects[4]) Mix_PlayChannel(1, sound_effects[4], 0);
                            break;
                        case SDLK_LEFT:
                        case SDLK_a:
                            snake.change_direction(DIR_LEFT);
                            if (sound_effects[4]) Mix_PlayChannel(1, sound_effects[4], 0);
                            break;
                        case SDLK_RIGHT:
                        case SDLK_d:
                            snake.change_direction(DIR_RIGHT);
                            if (sound_effects[4]) Mix_PlayChannel(1, sound_effects[4], 0);
                            break;
                        case SDLK_p:
                        case SDLK_ESCAPE:
                            state = STATE_PAUSED;
                            break;
                    }
                    break;
                    
                case STATE_PAUSED:
                    if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_ESCAPE) {
                        state = STATE_PLAYING;
                    }
                    break;
                    
                case STATE_GAME_OVER:
                    if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {
                        state = STATE_MENU;
                    }
                    break;
            }
        }
    }
}

void Game::update() {
    if (state != STATE_PLAYING) return;
    
    Uint32 current_time = SDL_GetTicks();
    
    // Update power-ups
    power_ups.update(current_time);
    
    // Update food animation
    food.pulse_phase += 0.1f;
    if (food.pulse_phase > 2 * M_PI) {
        food.pulse_phase = 0;
    }
    food.glow_intensity = 0.8f + 0.2f * sinf(food.pulse_phase);
    
    // Update particles
    update_particles();
    
    // Update screen shake
    if (current_time < screen_shake_end_time) {
        float shake_progress = (float)(screen_shake_end_time - current_time) / 500.0f;
        screen_shake_intensity *= shake_progress;
    } else {
        screen_shake_intensity = 0.0f;
    }
    
    // Determine current move delay based on power-ups and level
    Uint32 current_move_delay = get_level_speed(level, power_ups.is_speed_active());
    
    // Move snake based on timing
    if (current_time - last_move_time >= current_move_delay) {
        snake.move();
        last_move_time = current_time;
        
        // Check wall collision (with phase mode support)
        if (snake.check_collision(power_ups.is_phase_active())) {
            game_over_alpha = 0;
            if (score > high_score) {
                high_score = score;
                save_high_score();
            }
            
            // Update game statistics and check achievements
            Uint32 game_duration = SDL_GetTicks() - game_start_time;
            game_stats->update_game_end(score, level, snake.get_length(),
                                     foods_eaten, special_foods_eaten,
                                     power_ups.combo_multiplier, game_duration);
            
            auto new_achievements = achievement_system->update(*game_stats);
            // TODO: Display achievement notifications
            
            if (sound_effects[21]) {
                Mix_PlayChannel(-1, sound_effects[21], 0);
            }
            state = STATE_GAME_OVER;
            return;
        }
        
        // Check collision with food
        if (food.check_collision(snake)) {
            // Spawn particles at food location
            spawn_food_particles(food.x * GRID_SIZE + GRID_SIZE/2, 
                                food.y * GRID_SIZE + GRID_SIZE/2, food.type);
            spawn_power_up_particles(food.x * GRID_SIZE + GRID_SIZE/2,
                                   food.y * GRID_SIZE + GRID_SIZE/2, food.type);
            
            // Apply food effect
            apply_food_effect(food.type);
            
            // Spawn new food
            food.spawn(snake);
            food.type = food.get_random_type(level, foods_eaten);
            
            foods_eaten++;
            
            // Check if level up
            if (foods_eaten >= foods_needed_for_level) {
                level_up();
            }
            
            // Play eat sound
            if (sound_effects[3]) {
                Mix_PlayChannel(1, sound_effects[3], 0);
            }
        }
    }
}

void Game::apply_food_effect(FoodType type) {
    Uint32 current_time = SDL_GetTicks();
    
    // Update combo system
    power_ups.combo_count++;
    power_ups.combo_multiplier = 1 + (power_ups.combo_count / 3); // Increase every 3 foods
    power_ups.last_food_time = current_time;
    
    int base_points = base_score_per_food;
    int multiplier = power_ups.get_score_multiplier();
    
    switch (type) {
        case FOOD_NORMAL:
            snake.grow();
            score += base_points * multiplier;
            break;
            
        case FOOD_SPEED:
            snake.grow();
            score += (base_points + 5) * multiplier;
            power_ups.speed_boost = true;
            power_ups.speed_end_time = current_time + 5000; // 5 seconds
            special_foods_eaten++;
            break;
            
        case FOOD_DOUBLE:
            snake.grow();
            score += base_points * multiplier;
            power_ups.double_score = true;
            power_ups.double_score_end_time = current_time + 8000; // 8 seconds
            special_foods_eaten++;
            break;
            
        case FOOD_GOLDEN:
            snake.grow();
            score += (base_points * 3) * multiplier;
            special_foods_eaten++;
            break;
            
        case FOOD_SHRINK:
            snake.shrink(2);
            score += (base_points / 2) * multiplier;
            special_foods_eaten++;
            break;
            
        case FOOD_PHASE:
            snake.grow();
            score += (base_points + 10) * multiplier;
            power_ups.phase_through_walls = true;
            power_ups.phase_end_time = current_time + 6000; // 6 seconds
            special_foods_eaten++;
            break;
            
        case FOOD_MEGA:
            snake.grow();
            score += (base_points * 5) * multiplier;
            add_screen_shake(10.0f, 500);
            special_foods_eaten++;
            break;
    }
}

void Game::level_up() {
    level++;
    foods_needed_for_level = get_level_required_foods(level);
    foods_eaten = 0;
    
    // Increase base score per food
    base_score_per_food += 2;
    
    add_screen_shake(5.0f, 300);
    
    if (sound_effects[10]) {
        Mix_PlayChannel(-1, sound_effects[10], 0);
    }
}

Uint32 Game::get_level_speed(int level, bool speed_boost) {
    Uint32 speed = base_move_delay - (level - 1) * 10;
    speed = std::max(speed, 50u); // Minimum delay
    
    if (speed_boost) {
        speed = speed * 0.6f; // 40% faster
    }
    
    return speed;
}

int Game::get_level_required_foods(int level) {
    return 5 + (level - 1) * 2; // 5, 7, 9, 11, ...
}

void Game::add_screen_shake(float intensity, Uint32 duration) {
    screen_shake_intensity = intensity;
    screen_shake_end_time = SDL_GetTicks() + duration;
}

void Game::reset() {
    snake.init();
    food.spawn(snake);
    food.type = food.get_random_type(1, 0);
    power_ups.init();
    
    score = 0;
    level = 1;
    foods_needed_for_level = 5;
    foods_eaten = 0;
    special_foods_eaten = 0;
    base_score_per_food = 10;
    game_start_time = SDL_GetTicks();
    
    move_delay = base_move_delay;
    last_move_time = 0;
    food_pulse = 0;
    game_over_alpha = 0;
    screen_shake_intensity = 0.0f;
    screen_shake_end_time = 0;
    
    particles.clear();
}

// Rendering methods
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 35, 255);
    SDL_RenderClear(renderer);
    
    switch (state) {
        case STATE_MENU:
            render_menu();
            break;
        case STATE_PLAYING:
            render_game();
            break;
        case STATE_PAUSED:
            render_game();
            render_text("PAUSED - Press P to continue", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2, {255, 255, 0, 255});
            break;
        case STATE_GAME_OVER:
            render_game_over();
            break;
    }
    
    SDL_RenderPresent(renderer);
}

void Game::render_loading_screen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    if (loading_texture) {
        SDL_Rect dest = {0, 0, SCREEN_WIDTH + 200, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, loading_texture, nullptr, &dest);
    } else {
        // Fallback text rendering for BELLO DEV
        render_text("BELLO DEV", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 30, {255, 255, 255, 255}, large_font);
        render_text("Snake Game - Revolutionary Edition", SCREEN_WIDTH/2 - 160, SCREEN_HEIGHT/2 + 20, {200, 200, 200, 255});
    }
    
    SDL_RenderPresent(renderer);
}

void Game::render_menu() {
    render_gradient_background();
    
    // Animated title with glow effect
    float time = SDL_GetTicks() / 1000.0f;
    float title_glow = 0.8f + 0.2f * sinf(time * 2);
    
    SDL_Color title_color = {
        static_cast<Uint8>(255 * title_glow),
        static_cast<Uint8>(200 * title_glow),
        static_cast<Uint8>(100 * title_glow),
        255
    };
    
    render_text("SNAKE SDL2", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 - 120, title_color, large_font);
    
    // Enhanced subtitle with color cycling
    Uint8 subtitle_r = 150 + (Uint8)(50 * sinf(time));
    Uint8 subtitle_g = 200 + (Uint8)(55 * sinf(time + 2));
    Uint8 subtitle_b = 255;
    render_text("Revolutionary Edition with Magic Powers!", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 - 70, {subtitle_r, subtitle_g, subtitle_b, 255});
    
    // Feature highlights
    render_text("✦ 7 Magic Food Types ✦ Power-Up System ✦", SCREEN_WIDTH/2 - 150, SCREEN_WIDTH/2 - 45, {255, 200, 150, 255});
    render_text("✦ Combo System ✦ Spectacular Visual Effects ✦", SCREEN_WIDTH/2 - 170, SCREEN_HEIGHT/2 - 25, {200, 255, 200, 255});
    
    // Difficulty selection
    render_text("Select Difficulty:", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 20, {255, 255, 255, 255});
    
    SDL_Color easy_color = (difficulty == DIFFICULTY_EASY) ? SDL_Color{255, 255, 0, 255} : SDL_Color{150, 150, 150, 255};
    SDL_Color normal_color = (difficulty == DIFFICULTY_NORMAL) ? SDL_Color{255, 255, 0, 255} : SDL_Color{150, 150, 150, 255};
    SDL_Color hard_color = (difficulty == DIFFICULTY_HARD) ? SDL_Color{255, 255, 0, 255} : SDL_Color{150, 150, 150, 255};
    
    render_text("1. Easy", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, easy_color);
    render_text("2. Normal", SCREEN_WIDTH/2 - 30, SCREEN_HEIGHT/2 + 50, normal_color);
    render_text("3. Hard", SCREEN_WIDTH/2 + 50, SCREEN_HEIGHT/2 + 50, hard_color);
    
    render_text("Press SPACE to start!", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 100, {255, 255, 255, 255});
    
    // High score display
    std::string high_score_text = "High Score: " + std::to_string(high_score);
    render_text(high_score_text, SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 130, {200, 200, 200, 255});
    
    // Food legend
    render_text("Food Types:", SCREEN_WIDTH + 20, 50, {255, 255, 255, 255});
    render_text("Green: Normal (+10)", SCREEN_WIDTH + 20, 80, {0, 255, 0, 255});
    render_text("Yellow: Speed Boost", SCREEN_WIDTH + 20, 100, {255, 255, 0, 255});
    render_text("Cyan: Double Score", SCREEN_WIDTH + 20, 120, {0, 255, 255, 255});
    render_text("Gold: High Bonus", SCREEN_WIDTH + 20, 140, {255, 215, 0, 255});
    render_text("Magenta: Shrink", SCREEN_WIDTH + 20, 160, {255, 0, 255, 255});
    render_text("Purple: Phase Mode", SCREEN_WIDTH + 20, 180, {128, 0, 255, 255});
    render_text("Red: MEGA Bonus", SCREEN_WIDTH + 20, 200, {255, 100, 100, 255});
}

void Game::render_game() {
    render_gradient_background();
    
    // Apply screen shake
    int shake_x = 0, shake_y = 0;
    if (screen_shake_intensity > 0) {
        shake_x = static_cast<int>((rand() % 21 - 10) * screen_shake_intensity / 10.0f);
        shake_y = static_cast<int>((rand() % 21 - 10) * screen_shake_intensity / 10.0f);
    }
    
    // Render game elements
    render_food();
    render_snake();
    render_particles();
    render_ui();
    render_power_up_indicators();
}

void Game::render_game_over() {
    render_gradient_background();
    
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, SCREEN_WIDTH + 200, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);
    
    // Game Over title
    render_text("GAME OVER", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 100, {255, 100, 100, 255}, large_font);
    
    // Statistics
    std::string score_text = "Final Score: " + std::to_string(score);
    render_text(score_text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 50, {255, 255, 255, 255});
    
    std::string level_text = "Level Reached: " + std::to_string(level);
    render_text(level_text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 20, {255, 255, 255, 255});
    
    std::string length_text = "Snake Length: " + std::to_string(snake.get_length());
    render_text(length_text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 10, {255, 255, 255, 255});
    
    // Statistics display
    std::string games_text = "Games Played: " + std::to_string(game_stats->games_played);
    render_text(games_text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 40, {200, 200, 200, 255});
    
    Uint32 game_time = (SDL_GetTicks() - game_start_time) / 1000;
    std::string game_time_text = "Game Time: " + std::to_string(game_time) + "s";
    render_text(game_time_text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 70, {200, 200, 200, 255});
    
    // Achievement progress
    int unlocked_count = achievement_system->get_unlocked_count();
    int total_count = achievement_system->get_total_count();
    std::string achievement_text = "Achievements: " + std::to_string(unlocked_count) + "/" + std::to_string(total_count);
    render_text(achievement_text, SCREEN_WIDTH/2 - 90, SCREEN_HEIGHT/2 + 130, {255, 215, 0, 255});
    
    if (score == high_score && score > 0) {
        render_text("NEW HIGH SCORE!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 160, {255, 255, 0, 255});
    }
    
    render_text("Press SPACE to return to menu", SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 + 190, {200, 200, 200, 255});
}

void Game::render_snake() {
    for (size_t i = 0; i < snake.segments.size(); i++) {
        const Segment& seg = snake.segments[i];
        SDL_Rect rect = {seg.x * GRID_SIZE, seg.y * GRID_SIZE, GRID_SIZE, GRID_SIZE};
        
        if (i == 0) { // Head
            // Head with special effects based on active powers
            SDL_Color head_color = {100, 255, 100, 255};
            if (power_ups.is_speed_active()) {
                head_color = {255, 255, 100, 255}; // Yellow for speed
            } else if (power_ups.is_phase_active()) {
                head_color = {200, 100, 255, 255}; // Purple for phase
            }
            
            SDL_SetRenderDrawColor(renderer, head_color.r, head_color.g, head_color.b, head_color.a);
            SDL_RenderFillRect(renderer, &rect);
            
            // Eyes
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_Rect eye1 = {rect.x + 2, rect.y + 2, 4, 4};
            SDL_Rect eye2 = {rect.x + GRID_SIZE - 6, rect.y + 2, 4, 4};
            SDL_RenderFillRect(renderer, &eye1);
            SDL_RenderFillRect(renderer, &eye2);
        } else {
            // Body with gradient effect
            float body_ratio = static_cast<float>(i) / snake.segments.size();
            Uint8 green_intensity = static_cast<Uint8>(255 * (1.0f - body_ratio * 0.5f));
            SDL_SetRenderDrawColor(renderer, 50, green_intensity, 50, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
        
        // Border
        SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void Game::render_food() {
    if (!food.active) return;
    
    SDL_Rect rect = {food.x * GRID_SIZE, food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE};
    
    // Pulsing effect
    float pulse_scale = 1.0f + 0.2f * sinf(food.pulse_phase);
    int pulse_offset = static_cast<int>((1.0f - pulse_scale) * GRID_SIZE / 2);
    rect.x += pulse_offset;
    rect.y += pulse_offset;
    rect.w = static_cast<int>(GRID_SIZE * pulse_scale);
    rect.h = static_cast<int>(GRID_SIZE * pulse_scale);
    
    SDL_Color color = food.get_color();
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    
    // Glow effect
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 50);
    SDL_Rect glow_rect = {rect.x - 2, rect.y - 2, rect.w + 4, rect.h + 4};
    SDL_RenderFillRect(renderer, &glow_rect);
}

void Game::render_ui() {
    // Score
    std::string score_text = "Score: " + std::to_string(score);
    render_text(score_text, SCREEN_WIDTH + 20, 20, {255, 255, 255, 255});
    
    // Level
    std::string level_text = "Level: " + std::to_string(level);
    render_text(level_text, SCREEN_WIDTH + 20, 50, {255, 255, 255, 255});
    
    // Progress to next level
    int progress = foods_eaten;
    int needed = foods_needed_for_level;
    std::string progress_text = "Progress: " + std::to_string(progress) + "/" + std::to_string(needed);
    render_text(progress_text, SCREEN_WIDTH + 20, 80, {200, 200, 255, 255});
    
    // Snake length
    std::string length_text = "Length: " + std::to_string(snake.get_length());
    render_text(length_text, SCREEN_WIDTH + 20, 110, {200, 255, 200, 255});
    
    // Combo multiplier
    if (power_ups.combo_multiplier > 1) {
        std::string combo_text = "Combo x" + std::to_string(power_ups.combo_multiplier);
        render_text(combo_text, SCREEN_WIDTH + 20, 140, {255, 255, 0, 255});
    }
    
    // Game time
    Uint32 game_time = (SDL_GetTicks() - game_start_time) / 1000;
    std::string time_text = "Time: " + std::to_string(game_time / 60) + ":" + 
                           (game_time % 60 < 10 ? "0" : "") + std::to_string(game_time % 60);
    render_text(time_text, SCREEN_WIDTH + 20, SCREEN_HEIGHT - 30, {200, 255, 200, 255});
}

void Game::render_power_up_indicators() {
    int y_offset = SCREEN_HEIGHT - 120;
    
    // Speed boost indicator
    if (power_ups.is_speed_active()) {
        render_text("SPEED BOOST", SCREEN_WIDTH + 20, y_offset, {255, 255, 0, 255});
        y_offset += 25;
    }
    
    // Double score indicator
    if (power_ups.is_double_score_active()) {
        render_text("DOUBLE SCORE", SCREEN_WIDTH + 20, y_offset, {0, 255, 255, 255});
        y_offset += 25;
    }
    
    // Phase indicator
    if (power_ups.is_phase_active()) {
        render_text("PHASE MODE", SCREEN_WIDTH + 20, y_offset, {255, 0, 255, 255});
        y_offset += 25;
    }
}

void Game::render_gradient_background() {
    // Create a dynamic gradient background with animated stars
    Uint32 current_time = SDL_GetTicks();
    float time_factor = sinf(current_time / 5000.0f) * 0.1f;
    
    // Draw gradient background
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float gradient = static_cast<float>(y) / SCREEN_HEIGHT;
        Uint8 r = static_cast<Uint8>(20 + gradient * 30 + time_factor * 10);
        Uint8 g = static_cast<Uint8>(20 + gradient * 40 + time_factor * 15);
        Uint8 b = static_cast<Uint8>(35 + gradient * 60 + time_factor * 20);
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
    
    // Draw animated stars
    for (int i = 0; i < 50; i++) {
        int star_x = (i * 37 + current_time / 50) % SCREEN_WIDTH;
        int star_y = (i * 53) % SCREEN_HEIGHT;
        float twinkle = sinf((current_time + i * 100) / 1000.0f) * 0.5f + 0.5f;
        Uint8 alpha = static_cast<Uint8>(100 * twinkle);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
        SDL_RenderDrawPoint(renderer, star_x, star_y);
        SDL_RenderDrawPoint(renderer, star_x + 1, star_y);
        SDL_RenderDrawPoint(renderer, star_x, star_y + 1);
    }
}

void Game::render_text(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font_to_use) {
    if (!font_to_use) font_to_use = font;
    if (!font_to_use) return;
    
    SDL_Surface* surface = TTF_RenderText_Solid(font_to_use, text.c_str(), color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) return;
    
    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    SDL_Rect dest = {x, y, w, h};
    
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
    SDL_DestroyTexture(texture);
}

// Particle system
void Game::add_particle(float x, float y, float vx, float vy, SDL_Color color) {
    if (particles.size() >= MAX_PARTICLES) return;
    
    Particle p;
    p.x = x;
    p.y = y;
    p.vx = vx;
    p.vy = vy;
    p.life = 1.0f + (rand() % 100) / 100.0f; // 1-2 seconds
    p.max_life = p.life;
    p.r = color.r;
    p.g = color.g;
    p.b = color.b;
    p.a = color.a;
    
    particles.push_back(p);
}

void Game::spawn_food_particles(float x, float y, FoodType type) {
    SDL_Color color = food.get_color();
    
    for (int i = 0; i < 10; i++) {
        float angle = static_cast<float>(i) * 2 * M_PI / 10;
        float speed = 50 + rand() % 50;
        float vx = cosf(angle) * speed;
        float vy = sinf(angle) * speed;
        
        add_particle(x, y, vx, vy, color);
    }
}

void Game::spawn_power_up_particles(float x, float y, FoodType type) {
    if (type == FOOD_NORMAL) return;
    
    SDL_Color color = {255, 255, 0, 255}; // Gold particles for special foods
    
    for (int i = 0; i < 15; i++) {
        float angle = static_cast<float>(rand()) / RAND_MAX * 2 * M_PI;
        float speed = 30 + rand() % 70;
        float vx = cosf(angle) * speed;
        float vy = sinf(angle) * speed;
        
        add_particle(x, y, vx, vy, color);
    }
}

void Game::update_particles() {
    float dt = 0.016f; // ~60fps
    
    for (auto it = particles.begin(); it != particles.end();) {
        it->update(dt);
        if (!it->is_alive()) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::render_particles() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    for (const auto& particle : particles) {
        SDL_SetRenderDrawColor(renderer, particle.r, particle.g, particle.b, particle.a);
        SDL_RenderDrawPoint(renderer, static_cast<int>(particle.x), static_cast<int>(particle.y));
        SDL_RenderDrawPoint(renderer, static_cast<int>(particle.x) + 1, static_cast<int>(particle.y));
        SDL_RenderDrawPoint(renderer, static_cast<int>(particle.x), static_cast<int>(particle.y) + 1);
    }
}

// Save/Load system
void Game::save_high_score() {
    std::ofstream file("highscore.dat");
    if (file.is_open()) {
        file << high_score;
        file.close();
    }
}

void Game::load_high_score() {
    std::ifstream file("highscore.dat");
    if (file.is_open()) {
        file >> high_score;
        file.close();
    }
}