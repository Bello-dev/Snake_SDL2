#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include <string>
#include <vector>
#include <SDL2/SDL.h>

enum AchievementType {
    ACH_SCORE,
    ACH_LENGTH,
    ACH_LEVEL,
    ACH_FOODS,
    ACH_SPECIAL_FOODS,
    ACH_GAMES_PLAYED,
    ACH_TIME_PLAYED,
    ACH_COMBO
};

struct Achievement {
    std::string name;
    std::string description;
    AchievementType type;
    int target_value;
    bool unlocked;
    int progress;
    
    Achievement(const std::string& name, const std::string& desc, 
               AchievementType type, int target);
    
    bool check_progress(const struct GameStats& stats);
    float get_progress_percent() const;
};

struct GameStats {
    int games_played;
    int high_score;
    int max_level;
    int max_length;
    long long total_score;
    Uint32 total_time_played; // in seconds
    int total_foods_eaten;
    int special_foods_eaten;
    int max_combo;
    
    GameStats();
    void update_game_end(int final_score, int final_level, int snake_length,
                        int foods_eaten, int special_foods, int max_combo_reached,
                        Uint32 game_duration);
    void save_stats();
    void load_stats();
    void reset_stats();
};

class AchievementSystem {
private:
    std::vector<Achievement> achievements;
    
    void init_achievements();
    void save_progress();
    void load_progress();
    
public:
    AchievementSystem();
    
    std::vector<std::string> update(const GameStats& stats);
    const std::vector<Achievement>& get_achievements() const { return achievements; }
    int get_unlocked_count() const;
    int get_total_count() const { return achievements.size(); }
};

#endif // ACHIEVEMENTS_H