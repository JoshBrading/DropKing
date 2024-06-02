#include "game.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace Game
{
    bool GameWorld::load_level(const char* level_name)
    {
        Level* level = new Level();
        std::string path = level_path + std::string(level_name) + ".json";
        std::filesystem::path current_path = std::filesystem::current_path();
        path = current_path.string() + "\\" + path;
        std::ifstream file(path);
        if( !file.is_open() )
        {
            std::cerr << "Failed to open file: " << path << std::endl;
            return false;
        }
        nlohmann::json data = nlohmann::json::parse(file);

        level->name = to_string(data["name"]).c_str();
        level->spawn_point = {data["spawn_point"]["x"], data["spawn_point"]["y"]};
        level->time_to_complete = data["time_to_complete"];
        level->end_height = data["end_height"];
        
        for( auto& wall : data["map_objects"]["walls"] )
        {
            float x = wall["start"]["x"];
            float y = wall["start"]["y"];
            float height = wall["height"];

            Physics::Object* obj = Physics::create_wall({x, y}, height);
            level->objects.push_back(obj);
        }
        for( auto& platform : data["map_objects"]["platforms"] )
        {
            float x = platform["start"]["x"];
            float y = platform["start"]["y"];
            float length = platform["length"];
            float deg = platform["angle"];

            Physics::Object* obj = Physics::create_platform({x, y}, length, deg);
            level->objects.push_back(obj);
        }
        active_level = level;
        return true;
    }

    Level* GameWorld::get_active_level()
    {
        return active_level;
    }

    void GameWorld::update()
    {
        if( active_level )
        {
            for( auto& obj : active_level->objects )
            {
                DrawLineEx(obj->start, obj->end, 4, GREEN);
            }
        }
    }
}
