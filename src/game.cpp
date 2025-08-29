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
               screen_shake_end_time(0), loading_progress(0), loading_start_time(0),
               game_stats(nullptr), achievement_system(nullptr) {
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
    // Initialize loading animation
    loading_start_time = SDL_GetTicks();
    loading_particles.clear();
    
    // Show loading screen with animation for 3 seconds
    Uint32 loading_duration = 3000;
    while (SDL_GetTicks() - loading_start_time < loading_duration) {
        // Handle quit events during loading
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                return;
            }
        }
        
        render_loading_screen();
        SDL_Delay(16); // ~60 FPS
    }
    
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
    // Calculate animation progress (0.0 to 1.0 over 3 seconds)
    float elapsed = (SDL_GetTicks() - loading_start_time) / 3000.0f;
    elapsed = std::min(elapsed, 1.0f);
    
    // Dynamic gradient background
    render_futuristic_background(elapsed);
    
    // Update and render loading particles
    update_loading_particles(elapsed);
    render_loading_particles();
    
    // BELLO DEV logo with spectacular effects
    render_animated_logo(elapsed);
    
    // Subtitle with typewriter effect
    render_animated_subtitle(elapsed);
    
    // Progress bar
    render_loading_progress(elapsed);
    
    SDL_RenderPresent(renderer);
}

void Game::render_futuristic_background(float progress) {
    // Dynamic gradient from dark blue to purple to pink
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float gradient = (float)y / SCREEN_HEIGHT;
        
        // Color transitions based on progress
        Uint8 r = static_cast<Uint8>(10 + (100 * progress) + (50 * sin(progress * 6.28f)) + gradient * 30);
        Uint8 g = static_cast<Uint8>(5 + (50 * progress) + gradient * 50);
        Uint8 b = static_cast<Uint8>(30 + (120 * progress) + (80 * sin(progress * 3.14f)) + gradient * 100);
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_Rect line = {0, y, SCREEN_WIDTH + 200, 1};
        SDL_RenderFillRect(renderer, &line);
    }
    
    // Add moving stars/sparkles
    int star_count = static_cast<int>(30 * progress);
    for (int i = 0; i < star_count; i++) {
        float star_time = progress * 2.0f + i * 0.1f;
        int x = static_cast<int>((50 + i * 13) % (SCREEN_WIDTH + 200)) + static_cast<int>(sin(star_time) * 20);
        int y = static_cast<int>((30 + i * 17) % SCREEN_HEIGHT) + static_cast<int>(cos(star_time * 1.5f) * 10);
        
        float alpha = sin(star_time * 3) * 0.5f + 0.5f;
        Uint8 star_alpha = static_cast<Uint8>(255 * alpha * progress);
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, star_alpha);
        SDL_Rect star = {x-1, y-1, 3, 3};
        SDL_RenderFillRect(renderer, &star);
    }
}

void Game::render_animated_logo(float progress) {
    if (progress < 0.2f) return; // Delay logo appearance
    
    float logo_progress = (progress - 0.2f) / 0.6f; // Logo animation from 0.2 to 0.8
    logo_progress = std::min(logo_progress, 1.0f);
    
    // Letter-by-letter reveal with glow
    std::string logo = "BELLO DEV";
    int base_x = SCREEN_WIDTH/2 - 100;
    int base_y = SCREEN_HEIGHT/2 - 50;
    
    for (size_t i = 0; i < logo.length(); i++) {
        float letter_progress = logo_progress * 9 - i; // Each letter appears slightly delayed
        if (letter_progress <= 0) continue;
        letter_progress = std::min(letter_progress, 1.0f);
        
        // Letter position with smooth animation
        int x = base_x + i * 20;
        int y = base_y + static_cast<int>(sin(letter_progress * 3.14f) * -30 * (1 - letter_progress));
        
        // Color with glow effect
        float glow_intensity = 0.7f + 0.3f * sin(SDL_GetTicks() / 200.0f + i);
        Uint8 r = static_cast<Uint8>(255 * glow_intensity * letter_progress);
        Uint8 g = static_cast<Uint8>(150 + 105 * glow_intensity * letter_progress);
        Uint8 b = static_cast<Uint8>(50 + 205 * glow_intensity * letter_progress);
        Uint8 a = static_cast<Uint8>(255 * letter_progress);
        
        // Multiple renders for glow effect
        for (int glow = 2; glow >= 0; glow--) {
            SDL_Color color = {r, g, b, static_cast<Uint8>(a / (glow + 1))};
            std::string letter(1, logo[i]);
            if (logo[i] != ' ') {
                render_text(letter, x - glow, y - glow, color, large_font);
                render_text(letter, x + glow, y + glow, color, large_font);
            }
        }
        
        SDL_Color final_color = {r, g, b, a};
        std::string letter(1, logo[i]);
        if (logo[i] != ' ') {
            render_text(letter, x, y, final_color, large_font);
        }
    }
}

void Game::render_animated_subtitle(float progress) {
    if (progress < 0.6f) return;
    
    float subtitle_progress = (progress - 0.6f) / 0.4f;
    subtitle_progress = std::min(subtitle_progress, 1.0f);
    
    std::string subtitle = "Revolutionary Snake Experience";
    int visible_chars = static_cast<int>(subtitle.length() * subtitle_progress);
    
    std::string visible_subtitle = subtitle.substr(0, visible_chars);
    
    // Typewriter effect with cursor
    SDL_Color subtitle_color = {
        200, 
        static_cast<Uint8>(200 + 55 * sin(SDL_GetTicks() / 300.0f)), 
        255, 
        static_cast<Uint8>(255 * subtitle_progress)
    };
    
    render_text(visible_subtitle, SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 + 10, subtitle_color);
    
    // Blinking cursor
    if (subtitle_progress < 1.0f && (SDL_GetTicks() / 500) % 2 == 0) {
        render_text("_", SCREEN_WIDTH/2 - 140 + visible_chars * 10, SCREEN_HEIGHT/2 + 10, subtitle_color);
    }
}

void Game::render_loading_progress(float progress) {
    if (progress < 0.3f) return;
    
    float bar_progress = (progress - 0.3f) / 0.7f;
    bar_progress = std::min(bar_progress, 1.0f);
    
    int bar_width = 300;
    int bar_height = 8;
    int bar_x = SCREEN_WIDTH/2 - bar_width/2;
    int bar_y = SCREEN_HEIGHT/2 + 80;
    
    // Background bar
    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 200);
    SDL_Rect bg_bar = {bar_x - 2, bar_y - 2, bar_width + 4, bar_height + 4};
    SDL_RenderFillRect(renderer, &bg_bar);
    
    // Progress bar with gradient
    int fill_width = static_cast<int>(bar_width * bar_progress);
    for (int i = 0; i < fill_width; i++) {
        float gradient = (float)i / bar_width;
        Uint8 r = static_cast<Uint8>(100 + 155 * gradient);
        Uint8 g = static_cast<Uint8>(200 - 100 * gradient);
        Uint8 b = static_cast<Uint8>(255 - 155 * gradient);
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_Rect fill = {bar_x + i, bar_y, 1, bar_height};
        SDL_RenderFillRect(renderer, &fill);
    }
    
    // Loading text
    std::string loading_text = "Loading Revolutionary Experience... " + 
                              std::to_string(static_cast<int>(bar_progress * 100)) + "%";
    SDL_Color text_color = {200, 200, 255, static_cast<Uint8>(255 * bar_progress)};
    render_text(loading_text, SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 100, text_color);
}

void Game::update_loading_particles(float progress) {
    // Add new particles based on progress
    if (loading_particles.size() < static_cast<size_t>(50 * progress)) {
        Particle p;
        p.x = static_cast<float>(rand() % (SCREEN_WIDTH + 200));
        p.y = static_cast<float>(SCREEN_HEIGHT + 10);
        p.vx = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2;
        p.vy = -static_cast<float>(rand() % 3 + 1);
        p.life = p.max_life = 3.0f + static_cast<float>(rand() % 3);
        p.r = 100 + rand() % 156;
        p.g = 150 + rand() % 106;
        p.b = 200 + rand() % 56;
        p.a = 255;
        loading_particles.push_back(p);
    }
    
    // Update existing particles
    for (auto it = loading_particles.begin(); it != loading_particles.end();) {
        it->update(0.016f); // Assuming ~60fps
        if (!it->is_alive()) {
            it = loading_particles.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::render_loading_particles() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (const auto& p : loading_particles) {
        SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, p.a);
        SDL_Rect particle_rect = {
            static_cast<int>(p.x) - 1, 
            static_cast<int>(p.y) - 1, 
            3, 3
        };
        SDL_RenderFillRect(renderer, &particle_rect);
    }
}

void Game::render_menu() {
    render_futuristic_menu_background();
    
    // Spectacular animated title
    render_holographic_title();
    
    // Interactive difficulty selection with modern UI
    render_modern_difficulty_selection();
    
    // Advanced food showcase
    render_interactive_food_showcase();
    
    // Professional stats and info panel
    render_info_panel();
    
    // Futuristic controls hint
    render_futuristic_controls();
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
    float time = SDL_GetTicks() / 1000.0f;
    
    for (size_t i = 0; i < snake.segments.size(); i++) {
        const Segment& seg = snake.segments[i];
        SDL_Rect rect = {seg.x * GRID_SIZE, seg.y * GRID_SIZE, GRID_SIZE, GRID_SIZE};
        
        if (i == 0) { // Head - completely redesigned
            render_futuristic_snake_head(rect, time);
        } else {
            render_futuristic_snake_body(rect, i, time);
        }
        
        // Add connection joints between segments for smoother appearance
        if (i > 0) {
            render_snake_joint(i);
        }
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

// Enhanced menu rendering functions
void Game::render_futuristic_menu_background() {
    float time = SDL_GetTicks() / 1000.0f;
    
    // Dynamic layered background
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        float gradient = (float)y / SCREEN_HEIGHT;
        float wave = sin(time + gradient * 6.28f) * 0.1f;
        
        Uint8 r = static_cast<Uint8>(5 + (25 * gradient) + (15 * wave));
        Uint8 g = static_cast<Uint8>(10 + (40 * gradient) + (20 * sin(time * 0.7f + gradient * 3.14f)));
        Uint8 b = static_cast<Uint8>(40 + (100 * gradient) + (30 * wave));
        
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_Rect line = {0, y, SCREEN_WIDTH + 200, 1};
        SDL_RenderFillRect(renderer, &line);
    }
    
    // Animated grid pattern
    SDL_SetRenderDrawColor(renderer, 30, 50, 80, 100);
    for (int x = 0; x < SCREEN_WIDTH + 200; x += 40) {
        for (int y = 0; y < SCREEN_HEIGHT; y += 40) {
            float alpha_mod = sin(time + x * 0.01f + y * 0.01f) * 0.5f + 0.5f;
            if (alpha_mod > 0.7f) {
                SDL_Rect dot = {x + static_cast<int>(sin(time + x) * 2), 
                               y + static_cast<int>(cos(time + y) * 2), 2, 2};
                SDL_RenderFillRect(renderer, &dot);
            }
        }
    }
    
    // Floating energy orbs
    for (int i = 0; i < 8; i++) {
        float orb_time = time + i * 0.8f;
        int x = static_cast<int>(100 + (SCREEN_WIDTH - 200) * (sin(orb_time * 0.3f) + 1) / 2);
        int y = static_cast<int>(50 + (SCREEN_HEIGHT - 100) * (cos(orb_time * 0.2f) + 1) / 2);
        
        float glow = sin(orb_time * 2) * 0.3f + 0.7f;
        Uint8 alpha = static_cast<Uint8>(80 * glow);
        
        // Render orb with glow effect
        for (int radius = 8; radius > 0; radius -= 2) {
            SDL_SetRenderDrawColor(renderer, 
                                 100 + radius * 15, 
                                 150 + radius * 10, 
                                 255, 
                                 alpha / (radius / 2 + 1));
            for (int dx = -radius; dx <= radius; dx++) {
                for (int dy = -radius; dy <= radius; dy++) {
                    if (dx*dx + dy*dy <= radius*radius) {
                        SDL_Rect pixel = {x + dx, y + dy, 1, 1};
                        SDL_RenderFillRect(renderer, &pixel);
                    }
                }
            }
        }
    }
}

void Game::render_holographic_title() {
    float time = SDL_GetTicks() / 1000.0f;
    
    // Main title with holographic effect
    std::string title = "SNAKE REVOLUTION";
    int title_y = 80;
    
    for (size_t i = 0; i < title.length(); i++) {
        if (title[i] == ' ') continue;
        
        float letter_offset = sin(time * 3 + i * 0.3f) * 5;
        float glow_intensity = 0.8f + 0.2f * sin(time * 4 + i * 0.5f);
        
        int x = SCREEN_WIDTH/2 - 140 + i * 20;
        int y = title_y + static_cast<int>(letter_offset);
        
        // Multiple colored layers for holographic effect
        SDL_Color layers[] = {
            {static_cast<Uint8>(255 * glow_intensity), static_cast<Uint8>(100 * glow_intensity), static_cast<Uint8>(255 * glow_intensity), 255},
            {static_cast<Uint8>(100 * glow_intensity), static_cast<Uint8>(255 * glow_intensity), static_cast<Uint8>(255 * glow_intensity), 255},
            {static_cast<Uint8>(255 * glow_intensity), static_cast<Uint8>(255 * glow_intensity), static_cast<Uint8>(100 * glow_intensity), 255}
        };
        
        std::string letter(1, title[i]);
        
        // Render multiple layers for depth effect
        for (int layer = 0; layer < 3; layer++) {
            render_text(letter, x + layer - 1, y + layer - 1, layers[layer], large_font);
        }
    }
    
    // Subtitle with typewriter effect
    float subtitle_progress = fmod(time * 0.5f, 2.0f);
    if (subtitle_progress > 1.0f) subtitle_progress = 2.0f - subtitle_progress;
    
    std::string subtitle = "» FUTURE GAMING EXPERIENCE «";
    int visible_chars = static_cast<int>(subtitle.length() * subtitle_progress);
    std::string visible_subtitle = subtitle.substr(0, visible_chars);
    
    SDL_Color subtitle_color = {
        static_cast<Uint8>(std::min(255, 150 + static_cast<int>(105 * sin(time * 2)))),
        static_cast<Uint8>(std::min(255, 200 + static_cast<int>(55 * sin(time * 1.5f)))),
        255,
        255
    };
    
    render_text(visible_subtitle, SCREEN_WIDTH/2 - 140, 130, subtitle_color);
}

void Game::render_modern_difficulty_selection() {
    float time = SDL_GetTicks() / 1000.0f;
    int base_y = 200;
    
    // Section title
    render_text("◢ DIFFICULTY MATRIX ◣", SCREEN_WIDTH/2 - 90, base_y, {200, 255, 200, 255});
    
    const char* difficulty_names[] = {"APPRENTICE", "WARRIOR", "LEGEND"};
    const char* difficulty_keys[] = {"[1]", "[2]", "[3]"};
    const char* difficulty_desc[] = {
        "Perfect for learning", 
        "Balanced challenge", 
        "Ultimate test"
    };
    
    for (int i = 0; i < 3; i++) {
        bool selected = (difficulty == (i + 1));
        int y = base_y + 40 + i * 35;
        
        // Selection box with futuristic styling
        SDL_Color box_color;
        if (selected) {
            float pulse = sin(time * 8) * 0.3f + 0.7f;
            box_color = {
                static_cast<Uint8>(100 + 155 * pulse),
                static_cast<Uint8>(255 * pulse),
                static_cast<Uint8>(100 + 155 * pulse),
                200
            };
            
            // Animated border for selected difficulty
            SDL_Rect border = {SCREEN_WIDTH/2 - 150, y - 5, 300, 25};
            SDL_SetRenderDrawColor(renderer, box_color.r, box_color.g, box_color.b, 100);
            SDL_RenderFillRect(renderer, &border);
            
            // Side indicators
            render_text("►", SCREEN_WIDTH/2 - 170, y, box_color);
            render_text("◄", SCREEN_WIDTH/2 + 160, y, box_color);
        } else {
            box_color = {120, 120, 150, 255};
        }
        
        // Difficulty name and key
        render_text(difficulty_keys[i], SCREEN_WIDTH/2 - 140, y, box_color);
        render_text(difficulty_names[i], SCREEN_WIDTH/2 - 110, y, box_color);
        render_text(difficulty_desc[i], SCREEN_WIDTH/2 - 10, y, {150, 150, 170, 255});
    }
}

void Game::render_interactive_food_showcase() {
    float time = SDL_GetTicks() / 1000.0f;
    int panel_x = SCREEN_WIDTH + 20;
    int panel_y = 50;
    
    // Modern panel header
    render_text("═══ POWER CORES ═══", panel_x, panel_y, {255, 200, 100, 255});
    
    struct FoodInfo {
        const char* name;
        const char* effect;
        SDL_Color color;
        const char* symbol;
    };
    
    FoodInfo foods[] = {
        {"BASIC", "Standard Growth", {100, 255, 100, 255}, "●"},
        {"VELOCITY", "Speed Boost x1.6", {255, 255, 100, 255}, "⚡"},
        {"AMPLIFY", "Double Score x8s", {100, 255, 255, 255}, "◆"},
        {"AURUM", "Golden Bonus x3", {255, 215, 0, 255}, "★"},
        {"COMPACT", "Size Reduction", {255, 100, 255, 255}, "◇"},
        {"PHANTOM", "Phase Through", {150, 100, 255, 255}, "◈"},
        {"OMEGA", "Mega Bonus x5", {255, 100, 100, 255}, "⬟"}
    };
    
    for (int i = 0; i < 7; i++) {
        int y = panel_y + 30 + i * 25;
        float item_glow = sin(time * 2 + i * 0.5f) * 0.3f + 0.7f;
        
        SDL_Color glow_color = foods[i].color;
        glow_color.r = static_cast<Uint8>(glow_color.r * item_glow);
        glow_color.g = static_cast<Uint8>(glow_color.g * item_glow);
        glow_color.b = static_cast<Uint8>(glow_color.b * item_glow);
        
        // Animated food symbol
        render_text(foods[i].symbol, panel_x + 5, y, glow_color);
        render_text(foods[i].name, panel_x + 25, y, glow_color);
        render_text(foods[i].effect, panel_x + 25, y + 12, {180, 180, 200, 255});
    }
}

void Game::render_info_panel() {
    int panel_x = SCREEN_WIDTH/2 - 100;
    int panel_y = 350;
    
    // High score with animation
    float score_glow = sin(SDL_GetTicks() / 300.0f) * 0.2f + 0.8f;
    std::string high_score_text = "◈ RECORD: " + std::to_string(high_score) + " PTS ◈";
    SDL_Color score_color = {
        static_cast<Uint8>(255 * score_glow),
        static_cast<Uint8>(215 * score_glow),
        static_cast<Uint8>(0 * score_glow),
        255
    };
    render_text(high_score_text, panel_x, panel_y, score_color);
    
    // Achievement info
    int unlocked = achievement_system->get_unlocked_count();
    int total = achievement_system->get_total_count();
    std::string achievement_text = "◇ ACHIEVEMENTS: " + std::to_string(unlocked) + "/" + std::to_string(total) + " ◇";
    render_text(achievement_text, panel_x - 20, panel_y + 25, {150, 255, 150, 255});
}

void Game::render_futuristic_controls() {
    float time = SDL_GetTicks() / 1000.0f;
    int y = SCREEN_HEIGHT - 80;
    
    // Pulsing start instruction
    float pulse = sin(time * 4) * 0.3f + 0.7f;
    SDL_Color start_color = {
        static_cast<Uint8>(255 * pulse),
        static_cast<Uint8>(255 * pulse),
        static_cast<Uint8>(100 + 155 * pulse),
        255
    };
    
    render_text("▶ PRESS [SPACE] TO INITIALIZE ◀", SCREEN_WIDTH/2 - 130, y, start_color);
    
    // Controls hint
    render_text("WASD / Arrow Keys = Navigation | ESC = System Menu", 
                SCREEN_WIDTH/2 - 180, y + 25, {120, 150, 180, 255});
    
    // Version info
    render_text("v2.0 QUANTUM EDITION - by BELLO DEV", 
                SCREEN_WIDTH/2 - 120, y + 45, {100, 120, 140, 255});
}

// Enhanced Snake Rendering System
void Game::render_futuristic_snake_head(SDL_Rect rect, float time) {
    // Determine head direction for proper eye positioning
    Direction head_dir = snake.direction;
    
    // Base head colors with power-up modifications
    SDL_Color head_color = {80, 255, 120, 255}; // Default green
    SDL_Color glow_color = {40, 200, 80, 180};
    
    // Power-up color modifications
    if (power_ups.is_speed_active()) {
        head_color = {255, 220, 80, 255}; // Golden yellow
        glow_color = {255, 200, 0, 180};
    } else if (power_ups.is_phase_active()) {
        head_color = {180, 80, 255, 255}; // Purple
        glow_color = {150, 0, 255, 180};
    } else if (power_ups.is_double_score_active()) {
        head_color = {80, 200, 255, 255}; // Cyan
        glow_color = {0, 150, 255, 180};
    }
    
    // Pulsing effect
    float pulse = sin(time * 6) * 0.1f + 0.9f;
    head_color.r = static_cast<Uint8>(head_color.r * pulse);
    head_color.g = static_cast<Uint8>(head_color.g * pulse);
    head_color.b = static_cast<Uint8>(head_color.b * pulse);
    
    // Multiple layer rendering for depth
    for (int layer = 3; layer >= 0; layer--) {
        SDL_Rect layer_rect = {
            rect.x - layer,
            rect.y - layer,
            rect.w + layer * 2,
            rect.h + layer * 2
        };
        
        if (layer == 0) {
            // Core head
            SDL_SetRenderDrawColor(renderer, head_color.r, head_color.g, head_color.b, 255);
        } else {
            // Glow layers
            Uint8 layer_alpha = static_cast<Uint8>(glow_color.a / (layer + 1));
            SDL_SetRenderDrawColor(renderer, glow_color.r, glow_color.g, glow_color.b, layer_alpha);
        }
        
        SDL_RenderFillRect(renderer, &layer_rect);
    }
    
    // Advanced directional eyes
    render_snake_eyes(rect, head_dir, time);
    
    // Power-up indicators on head
    if (power_ups.is_speed_active()) {
        render_speed_indicators(rect, time);
    }
    
    // Head outline
    SDL_SetRenderDrawColor(renderer, 20, 100, 40, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void Game::render_futuristic_snake_body(SDL_Rect rect, size_t segment_index, float time) {
    float body_ratio = static_cast<float>(segment_index) / snake.segments.size();
    float wave_offset = sin(time * 3 + segment_index * 0.5f) * 3;
    
    // Adjust position for organic movement
    rect.x += static_cast<int>(wave_offset);
    
    // Gradient coloring from head to tail
    Uint8 base_green = static_cast<Uint8>(255 * (1.0f - body_ratio * 0.7f));
    Uint8 base_red = static_cast<Uint8>(50 + body_ratio * 30);
    Uint8 base_blue = static_cast<Uint8>(80 + body_ratio * 50);
    
    // Power-up color influences
    if (power_ups.is_speed_active()) {
        base_red = std::min(255, static_cast<int>(base_red * 1.5f));
        base_green = std::min(255, static_cast<int>(base_green * 1.2f));
    }
    
    if (power_ups.is_phase_active()) {
        base_blue = std::min(255, static_cast<int>(base_blue * 1.8f));
        base_red = std::min(255, static_cast<int>(base_red * 1.2f));
    }
    
    // Segmented body rendering
    SDL_Rect main_body = {rect.x + 1, rect.y + 1, rect.w - 2, rect.h - 2};
    SDL_SetRenderDrawColor(renderer, base_red, base_green, base_blue, 255);
    SDL_RenderFillRect(renderer, &main_body);
    
    // Scale-like texture
    for (int y = 0; y < rect.h; y += 4) {
        for (int x = 0; x < rect.w; x += 4) {
            if ((x + y) % 8 == 0) {
                Uint8 scale_intensity = static_cast<Uint8>(base_green * 0.8f);
                SDL_SetRenderDrawColor(renderer, base_red, scale_intensity, base_blue, 150);
                SDL_Rect scale = {rect.x + x, rect.y + y, 2, 2};
                SDL_RenderFillRect(renderer, &scale);
            }
        }
    }
    
    // Body outline
    SDL_SetRenderDrawColor(renderer, 20, static_cast<Uint8>(base_green * 0.6f), 30, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void Game::render_snake_eyes(SDL_Rect head_rect, Direction direction, float time) {
    int eye_size = 3;
    float blink = sin(time * 0.3f + 2) > 0.9f ? 0.5f : 1.0f; // Occasional blinking
    
    SDL_Rect eye1, eye2;
    
    // Position eyes based on direction
    switch (direction) {
        case DIR_UP:
            eye1 = {head_rect.x + 4, head_rect.y + 2, eye_size, static_cast<int>(eye_size * blink)};
            eye2 = {head_rect.x + head_rect.w - 7, head_rect.y + 2, eye_size, static_cast<int>(eye_size * blink)};
            break;
        case DIR_DOWN:
            eye1 = {head_rect.x + 4, head_rect.y + head_rect.h - 5, eye_size, static_cast<int>(eye_size * blink)};
            eye2 = {head_rect.x + head_rect.w - 7, head_rect.y + head_rect.h - 5, eye_size, static_cast<int>(eye_size * blink)};
            break;
        case DIR_LEFT:
            eye1 = {head_rect.x + 2, head_rect.y + 4, static_cast<int>(eye_size * blink), eye_size};
            eye2 = {head_rect.x + 2, head_rect.y + head_rect.h - 7, static_cast<int>(eye_size * blink), eye_size};
            break;
        case DIR_RIGHT:
        default:
            eye1 = {head_rect.x + head_rect.w - 5, head_rect.y + 4, static_cast<int>(eye_size * blink), eye_size};
            eye2 = {head_rect.x + head_rect.w - 5, head_rect.y + head_rect.h - 7, static_cast<int>(eye_size * blink), eye_size};
            break;
    }
    
    // Eye glow
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    SDL_RenderFillRect(renderer, &eye1);
    SDL_RenderFillRect(renderer, &eye2);
    
    // Eye pupils
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect pupil1 = {eye1.x + 1, eye1.y + 1, 1, 1};
    SDL_Rect pupil2 = {eye2.x + 1, eye2.y + 1, 1, 1};
    SDL_RenderFillRect(renderer, &pupil1);
    SDL_RenderFillRect(renderer, &pupil2);
}

void Game::render_speed_indicators(SDL_Rect head_rect, float time) {
    // Lightning bolts for speed boost
    float lightning_alpha = sin(time * 10) * 0.5f + 0.5f;
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, static_cast<Uint8>(255 * lightning_alpha));
    
    // Left lightning bolt
    SDL_Rect bolt1[] = {
        {head_rect.x - 6, head_rect.y + 2, 2, 4},
        {head_rect.x - 8, head_rect.y + 6, 2, 4},
        {head_rect.x - 4, head_rect.y + 10, 2, 4}
    };
    
    // Right lightning bolt
    SDL_Rect bolt2[] = {
        {head_rect.x + head_rect.w + 4, head_rect.y + 2, 2, 4},
        {head_rect.x + head_rect.w + 6, head_rect.y + 6, 2, 4},
        {head_rect.x + head_rect.w + 2, head_rect.y + 10, 2, 4}
    };
    
    for (int i = 0; i < 3; i++) {
        SDL_RenderFillRect(renderer, &bolt1[i]);
        SDL_RenderFillRect(renderer, &bolt2[i]);
    }
}

void Game::render_snake_joint(size_t segment_index) {
    if (segment_index == 0 || segment_index >= snake.segments.size()) return;
    
    const Segment& current = snake.segments[segment_index];
    const Segment& previous = snake.segments[segment_index - 1];
    
    // Calculate joint position between segments
    int joint_x = (current.x + previous.x) * GRID_SIZE / 2;
    int joint_y = (current.y + previous.y) * GRID_SIZE / 2;
    
    // Smooth transition joint
    SDL_SetRenderDrawColor(renderer, 60, 200, 80, 180);
    SDL_Rect joint = {joint_x - 2, joint_y - 2, 4, 4};
    SDL_RenderFillRect(renderer, &joint);
}