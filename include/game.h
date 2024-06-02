#pragma once
#include <vector>

#include "physics.h"

namespace Game
{
    struct Level
    {
        const char* name;
        const char* path;

        int time_to_complete;
        float end_height;
        Vector2 spawn_point;
        
        std::vector<Physics::Object*> objects;
        std::vector<Physics::Object*> platforms;
        std::vector<Physics::Object*> gems;
        std::vector<Physics::Object*> keys;
        std::vector<Physics::Object*> doors;
    };
    
    class GameWorld
    {
    public:
        void set_level_path(const char* path);
        
        bool load_level(const char* level_name);
        bool start_level(Level* level);
        
        Level* get_level(const char* level_name);
        
        Level* get_previous_level();
        Level* get_active_level();
        Level* get_next_level();
        
        std::vector<Level*> get_levels();
        
        void update();

    private:
        const char* level_path = "assets\\levels\\";
        int score = 0;
        std::vector<Level*> levels;
        Level* active_level = nullptr;
    };
}
