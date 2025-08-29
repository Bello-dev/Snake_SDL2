#include "achievements.h"
#include <iostream>
#include <fstream>

Achievement::Achievement(const std::string& name, const std::string& desc, 
                        AchievementType type, int target)
    : name(name), description(desc), type(type), target_value(target),
      unlocked(false), progress(0) {}

bool Achievement::check_progress(const GameStats& stats) {
    int old_progress = progress;
    
    switch (type) {
        case ACH_SCORE:
            progress = stats.high_score;
            break;
        case ACH_LENGTH:
            progress = stats.max_length;
            break;
        case ACH_LEVEL:
            progress = stats.max_level;
            break;
        case ACH_FOODS:
            progress = stats.total_foods_eaten;
            break;
        case ACH_SPECIAL_FOODS:
            progress = stats.special_foods_eaten;
            break;
        case ACH_GAMES_PLAYED:
            progress = stats.games_played;
            break;
        case ACH_TIME_PLAYED:
            progress = stats.total_time_played / 60; // Convert to minutes
            break;
        case ACH_COMBO:
            progress = stats.max_combo;
            break;
    }
    
    if (!unlocked && progress >= target_value) {
        unlocked = true;
        return true; // Just unlocked
    }
    
    return false; // No new unlock
}

float Achievement::get_progress_percent() const {
    if (unlocked) return 100.0f;
    return std::min(100.0f, (float(progress) / float(target_value)) * 100.0f);
}

// AchievementSystem implementation
AchievementSystem::AchievementSystem() {
    init_achievements();
    load_progress();
}

void AchievementSystem::init_achievements() {
    // Score-based achievements
    achievements.emplace_back("First Steps", "Score your first 100 points", ACH_SCORE, 100);
    achievements.emplace_back("Getting Good", "Reach 1,000 points", ACH_SCORE, 1000);
    achievements.emplace_back("Snake Master", "Reach 5,000 points", ACH_SCORE, 5000);
    achievements.emplace_back("Legend", "Reach 10,000 points", ACH_SCORE, 10000);
    achievements.emplace_back("Godlike", "Reach 25,000 points", ACH_SCORE, 25000);
    
    // Length-based achievements  
    achievements.emplace_back("Growing Up", "Reach length of 10", ACH_LENGTH, 10);
    achievements.emplace_back("Big Snake", "Reach length of 25", ACH_LENGTH, 25);
    achievements.emplace_back("Huge Snake", "Reach length of 50", ACH_LENGTH, 50);
    achievements.emplace_back("Colossal", "Reach length of 100", ACH_LENGTH, 100);
    
    // Level-based achievements
    achievements.emplace_back("Leveling Up", "Reach level 5", ACH_LEVEL, 5);
    achievements.emplace_back("Experienced", "Reach level 10", ACH_LEVEL, 10);
    achievements.emplace_back("Expert", "Reach level 20", ACH_LEVEL, 20);
    achievements.emplace_back("Unstoppable", "Reach level 50", ACH_LEVEL, 50);
    
    // Food-based achievements
    achievements.emplace_back("Hungry", "Eat 50 foods", ACH_FOODS, 50);
    achievements.emplace_back("Voracious", "Eat 200 foods", ACH_FOODS, 200);
    achievements.emplace_back("Omnivore", "Eat 500 foods", ACH_FOODS, 500);
    achievements.emplace_back("Foodie", "Eat 1000 foods", ACH_FOODS, 1000);
    
    // Special food achievements
    achievements.emplace_back("Magic Taste", "Eat 10 special foods", ACH_SPECIAL_FOODS, 10);
    achievements.emplace_back("Power Hunter", "Eat 50 special foods", ACH_SPECIAL_FOODS, 50);
    achievements.emplace_back("Gourmet", "Eat 100 special foods", ACH_SPECIAL_FOODS, 100);
    
    // Play time achievements
    achievements.emplace_back("Casual Player", "Play for 30 minutes", ACH_TIME_PLAYED, 30);
    achievements.emplace_back("Dedicated", "Play for 2 hours", ACH_TIME_PLAYED, 120);
    achievements.emplace_back("Addicted", "Play for 10 hours", ACH_TIME_PLAYED, 600);
    
    // Combo achievements
    achievements.emplace_back("Combo Starter", "Achieve 5x combo", ACH_COMBO, 5);
    achievements.emplace_back("Combo Master", "Achieve 10x combo", ACH_COMBO, 10);
    achievements.emplace_back("Combo God", "Achieve 15x combo", ACH_COMBO, 15);
    
    // Game count achievements
    achievements.emplace_back("Persistent", "Play 10 games", ACH_GAMES_PLAYED, 10);
    achievements.emplace_back("Determined", "Play 50 games", ACH_GAMES_PLAYED, 50);
    achievements.emplace_back("Never Give Up", "Play 100 games", ACH_GAMES_PLAYED, 100);
}

std::vector<std::string> AchievementSystem::update(const GameStats& stats) {
    std::vector<std::string> new_achievements;
    
    for (auto& achievement : achievements) {
        if (achievement.check_progress(stats)) {
            new_achievements.push_back(achievement.name);
            std::cout << "🏆 Achievement Unlocked: " << achievement.name << std::endl;
            std::cout << "   " << achievement.description << std::endl;
        }
    }
    
    if (!new_achievements.empty()) {
        save_progress();
    }
    
    return new_achievements;
}

void AchievementSystem::save_progress() {
    std::ofstream file("achievements.dat");
    if (!file.is_open()) return;
    
    for (const auto& achievement : achievements) {
        file << achievement.name << "|" 
             << achievement.unlocked << "|" 
             << achievement.progress << std::endl;
    }
}

void AchievementSystem::load_progress() {
    std::ifstream file("achievements.dat");
    if (!file.is_open()) return;
    
    std::string line;
    size_t index = 0;
    
    while (std::getline(file, line) && index < achievements.size()) {
        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1 + 1);
        
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            achievements[index].unlocked = (line.substr(pos1 + 1, pos2 - pos1 - 1) == "1");
            achievements[index].progress = std::stoi(line.substr(pos2 + 1));
        }
        index++;
    }
}

int AchievementSystem::get_unlocked_count() const {
    int count = 0;
    for (const auto& achievement : achievements) {
        if (achievement.unlocked) count++;
    }
    return count;
}

// GameStats implementation
GameStats::GameStats() {
    load_stats();
}

void GameStats::update_game_end(int final_score, int final_level, int snake_length,
                               int foods_eaten, int special_foods, int max_combo_reached,
                               Uint32 game_duration) {
    games_played++;
    total_score += final_score;
    total_time_played += game_duration / 1000; // Convert to seconds
    total_foods_eaten += foods_eaten;
    special_foods_eaten += special_foods;
    
    // Update maximums
    if (final_score > high_score) high_score = final_score;
    if (final_level > max_level) max_level = final_level;
    if (snake_length > max_length) max_length = snake_length;
    if (max_combo_reached > max_combo) max_combo = max_combo_reached;
    
    save_stats();
}

void GameStats::save_stats() {
    std::ofstream file("game_stats.dat");
    if (!file.is_open()) return;
    
    file << games_played << std::endl
         << high_score << std::endl
         << max_level << std::endl
         << max_length << std::endl
         << total_score << std::endl
         << total_time_played << std::endl
         << total_foods_eaten << std::endl
         << special_foods_eaten << std::endl
         << max_combo << std::endl;
}

void GameStats::load_stats() {
    std::ifstream file("game_stats.dat");
    if (!file.is_open()) {
        // Initialize with defaults
        games_played = 0;
        high_score = 0;
        max_level = 0;
        max_length = 0;
        total_score = 0;
        total_time_played = 0;
        total_foods_eaten = 0;
        special_foods_eaten = 0;
        max_combo = 0;
        return;
    }
    
    file >> games_played >> high_score >> max_level >> max_length
         >> total_score >> total_time_played >> total_foods_eaten
         >> special_foods_eaten >> max_combo;
}

void GameStats::reset_stats() {
    games_played = 0;
    high_score = 0;
    max_level = 0;
    max_length = 0;
    total_score = 0;
    total_time_played = 0;
    total_foods_eaten = 0;
    special_foods_eaten = 0;
    max_combo = 0;
    save_stats();
}