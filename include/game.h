#pragma once
#include <vector>
#include <xstring>

#include "physics.h"

namespace Game
{
    enum State
    {
        
    };
    struct Level
    {
        const char* name = "";
        const char* path = "";

        int time_to_complete = 0;
        float end_height = 0;
        Vector2 spawn_point = {0, 0};
        
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

        void load_levels();
        bool load_level(std::string level_name);
        bool start_level(Level* level);
        bool cleanup_level(Level* level);
        
        Level* get_level(const char* level_name);
        
        Level* get_previous_level();
        Level* get_active_level();
        Level* get_next_level();

        void add_level(Level* level);

        void start();
        
        std::vector<Level*> get_levels();
        
        void update();
        void draw();

    private:
        const char* level_path = "assets\\levels\\";
        int score = 0;
        bool is_paused = false;
        std::vector<Level*> levels;
        Level* active_level = nullptr;
        
    };
}
