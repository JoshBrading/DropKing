#include "game.h"
#include "player.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "finish_box.h"
#include "key.h"
#include "timed_platform.h"

namespace Game
{
    GameWorld::GameWorld(Camera2D* camera)
    {
        this->camera = camera;
    }

    void GameWorld::load_levels()
    {
        for (const auto & file : std::filesystem::directory_iterator(level_path))
        {
            if( file.path().extension() == ".json" )
            {
                std::string level_name = file.path().stem().string();
                load_level(level_name);
            }
        }
    }

    bool GameWorld::load_level(std::string level_name)
    {
        //load_levels_at_path(level_name);
        std::cout << "Loading level: " << level_name << '\n';
        Level* level = new Level();
        std::string path = level_path + level_name + ".json";
        std::cout << "Reading file: " << path << '\n';
        std::ifstream file(path);
        if( !file.is_open() )
        {
            std::cerr << "Failed to open file: " << path << '\n';
            return false;
        }
        nlohmann::json data = nlohmann::json::parse(file);

        //std::string name = data["name"];
        level->name = data["name"].get<std::string>().c_str();
        level->spawn_point = {data["spawn_point"]["x"], data["spawn_point"]["y"]};

        level->player = new Entities::Player(level->spawn_point);
        
        level->time_to_complete = data["time_to_complete"];

        level->finish_box = new Entities::FinishBox({data["finish_box"]["start"]["x"], data["finish_box"]["start"]["y"]}, {data["finish_box"]["end"]["x"], data["finish_box"]["end"]["y"]}, this);
        
        std::cout << "Loading walls \n";
        for( auto& wall : data["map_objects"]["walls"] )
        {
            float x = wall["start"]["x"];
            float y = wall["start"]["y"];
            float height = wall["height"];

            Physics::Object* obj = Physics::create_wall({x, y}, height);
            level->objects.push_back(obj);
        }
        Physics::Object* obj = Physics::create_wall({-500, 0}, 10000);
        level->objects.push_back(obj);
        Physics::Object* w2 = Physics::create_wall({500, 0}, 10000);
        level->objects.push_back(w2);
        std::cout << "Loading platforms \n";
        for( auto& platform : data["map_objects"]["platforms"] )
        {
            float start_x = platform["start"]["x"];
            float start_y = platform["start"]["y"];
            float end_x = platform["end"]["x"];
            float end_y = platform["end"]["y"];
            
            //Physics::Object* obj = Physics::create_platform({start_x, start_y}, {end_x, end_y});
            auto ent_platform = new Entities::Obstacles::Platform({start_x, start_y}, {end_x, end_y});
            level->objects.push_back(ent_platform->get_platform());
            level->platforms.push_back(ent_platform);
        }
        for(auto& timed_platform : data["obstacles"]["timed_platforms"])
        {
            float start_x = timed_platform["start"]["x"];
            float start_y = timed_platform["start"]["y"];
            float end_x = timed_platform["end"]["x"];
            float end_y = timed_platform["end"]["y"];
            int lifetime = timed_platform["time"];
            auto ent_platform = new Entities::Obstacles::TimedPlatform({start_x, start_y}, {end_x, end_y}, lifetime);
            level->objects.push_back(ent_platform->get_platform());
            level->timed_platforms.push_back(ent_platform);
        }
        for( auto& false_platform : data["obstacles"]["false_platforms"] )
        {
            float start_x = false_platform["start"]["x"];
            float start_y = false_platform["start"]["y"];
            float end_x = false_platform["end"]["x"];
            float end_y = false_platform["end"]["y"];
            
            auto ent_platform = new Entities::Obstacles::Platform({start_x, start_y}, {end_x, end_y});
            level->false_platforms.push_back(ent_platform);
        }

        for( auto& gem : data["items"]["gems"] )
        {
            float x = gem["position"]["x"];
            float y = gem["position"]["y"];
            auto ent_gem = new Entities::Items::Gem({x, y});
            level->objects.push_back(ent_gem->get_gem_object());
            level->gems.push_back(ent_gem);
        }
        std::cout << "Finished loading level \n";
        levels.push_back(level);
        return true;
    }

    bool GameWorld::start_level(Level* level)
    {
        if( !level )
        {
            std::cerr << "Level does not exist \n";
            return false;
        }
        
        if( active_level )
            cleanup_level(active_level);
        active_level = level;
        std::cout << "Starting level: " << level->name << '\n';
        std::cout << "Setting up physics bodies \n";
        for( auto& obj : level->objects )
        {
            Physics::add_object_to_physics(obj);
        }
        for(auto& platform : level->timed_platforms)
        {
            platform->reset();
        }

        for( auto& gem : level->gems )
        {
            gem->reset();
        }

        for( auto& key : level->keys )
        {
            key->reset();
        }
        add_object_to_physics(level->player->get_player_object());
        if( level->finish_box )
            add_object_to_physics(level->finish_box->get_finish_box());
        level->player->reset_player();
        std::cout << "Level started \n";
        return true;
    }

    bool GameWorld::cleanup_level(Level* level)
    {
        if( !level )
        {
            std::cerr << "Level does not exist \n";
            return false;
        }
        if( cpSpaceContainsBody(Physics::Instances::SPACE, level->player->get_player_object()->body))
            cpSpaceRemoveBody(Physics::Instances::SPACE, level->player->get_player_object()->body);
        if( cpSpaceContainsShape(Physics::Instances::SPACE, level->player->get_player_object()->shape))
            cpSpaceRemoveShape(Physics::Instances::SPACE, level->player->get_player_object()->shape);
        if( level->finish_box )
        {
            if( cpSpaceContainsShape(Physics::Instances::SPACE, level->finish_box->get_finish_box()->shape) )
                cpSpaceRemoveShape(Physics::Instances::SPACE, level->finish_box->get_finish_box()->shape);
        }
        
        for( auto& obj : level->objects )
        {
            if( cpSpaceContainsShape(Physics::Instances::SPACE, obj->shape))
                cpSpaceRemoveShape(Physics::Instances::SPACE, obj->shape);
        }
        active_level = nullptr;
        std::cout << "Level cleaned up \n";
        return true;
    }

    Level* GameWorld::get_active_level()
    {
        if(!active_level)
            return nullptr;
        return active_level;
    }

    void GameWorld::add_level(Level* level)
    {
        levels.push_back(level);
    }

    void GameWorld::start()
    {
        if( levels.empty() )
        {
            std::cerr << "No levels loaded \n";
            return;
        }
        if( active_level ) // Restart active level
            start_level(active_level);
        else               // Or start first level
            start_level(levels[0]);
    }

    std::vector<Level*> GameWorld::get_levels()
    {
        return levels;
    }

    void GameWorld::update()
    {
        if( active_level )
        {
            if( IsKeyPressed(KEY_R))
            {
                active_level->player->reset_player();
                start();
            }
            active_level->player->update();
            camera->target.x += (active_level->player->get_position().x - camera->target.x) * 0.01;
            camera->target.y = active_level->player->get_position().y;
            for( auto& platform : active_level->platforms )
            {
                platform->update();
            }
            for( auto& timed_platform : active_level->timed_platforms )
            {
                timed_platform->update();
            }
            for( auto& gem : active_level->gems )
            {
                gem->update();
            }
        }
    }

    void GameWorld::draw()
    {
        if( active_level )
        {
            for( auto& obj : active_level->objects )
            {
                if( obj->type == Physics::Shapes::WALL)
                    DrawLineEx(obj->start, obj->end, 4, {255, 255, 255, 255});
            }
            for( auto& platform : active_level->platforms )
            {
                platform->draw();
            }
            for( auto& timed_platform : active_level->timed_platforms )
            {
                timed_platform->draw();
            }
            for( auto& false_platform : active_level->false_platforms )
            {
                false_platform->draw();
            }
            for( auto& gem : active_level->gems )
            {
                gem->draw();
            }
            active_level->player->draw();
            if( active_level->finish_box )
                active_level->finish_box->draw();
        }
    }
}
