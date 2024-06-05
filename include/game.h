#pragma once
#include <vector>
#include "entity.h"
#include "gem.h"
#include "physics.h"
#include "player.h"
#include "timed_platform.h"
#include "platform.h"

namespace Game
{
    namespace Entities
    {
        namespace Items
        {
            class Key;
        }

        class FinishBox;
    }

    inline bool PAUSE = true;
    struct Level
    {
        const char* name = "";
        const char* path = "";

        int time_to_complete = 0;
        float end_height = 0;
        Vector2 spawn_point = {0, 0};
        bool camera_follows_player = true;
        Entities::Player* player = nullptr;
        Entities::FinishBox* finish_box = nullptr;
        std::vector<Physics::Object*> objects;
        std::vector<Entities::Obstacles::Platform*> platforms;
        std::vector<Entities::Obstacles::TimedPlatform*> timed_platforms;
        std::vector<Entities::Obstacles::Platform*> false_platforms;
        std::vector<Entities::Items::Gem*> gems;
        std::vector<Entities::Items::Key*> keys;
        std::vector<Physics::Object*> doors;
    };
    
    class GameWorld
    {
    public:
        GameWorld(Camera2D* camera);
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
        Camera2D* camera = nullptr;
        const char* level_path = "assets\\levels\\";
        int score = 0;
        bool is_paused = false;
        std::vector<Level*> levels;
        Level* active_level = nullptr;
        
    };
}
