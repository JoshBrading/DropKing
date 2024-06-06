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
        level_complete_menu = new Menu();
        std::string label = "Level Complete!";
        int font_size = 20;
        float text_width = MeasureText(label.c_str(), font_size);
        level_complete_menu->add_label(label, GetFontDefault(), font_size, {(GetScreenWidth() - text_width) / 2.0f , 400});
        level_complete_menu->add_button("Replay", GetFontDefault(), {(GetScreenWidth() - 200) / 2.0f, 500}, 100, 10, [this](Menu*, void* data)
        {
            start();
            level_complete_menu->close();
        });
        level_complete_menu->add_button("Next Level", GetFontDefault(), {(GetScreenWidth() - 200) / 2.0f, 525}, 100, 10, [this](Menu*, void* data)
        {
            start_next_level();
            level_complete_menu->close();
        });
        level_complete_menu->add_button("Quit", GetFontDefault(), {(GetScreenWidth() - 200) / 2.0f, 550}, 100, 10, [this](Menu*, void* data)
        {
            exit(0);
        });

        heart_filled_texture = LoadTexture("assets\\gui\\heart_filled.png");
        heart_empty_texture = LoadTexture("assets\\gui\\heart_empty.png");
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
        level->name = TextFormat("%s", data["name"]);
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

        for( auto& spike : data["obstacles"]["spikes"] )
        {
            float start_x = spike["start"]["x"];
            float start_y = spike["start"]["y"];
            float end_x = spike["end"]["x"];
            float end_y = spike["end"]["y"];
            auto ent_spike = new Entities::Obstacles::Spikes({start_x, start_y}, {end_x, end_y});
            level->objects.push_back(ent_spike->get_spikes());
            level->spikes.push_back(ent_spike);
        }

        for( auto& gem : data["items"]["gems"] )
        {
            float x = gem["position"]["x"];
            float y = gem["position"]["y"];
            auto ent_gem = new Entities::Items::Gem({x, y});
            level->objects.push_back(ent_gem->get_gem_object());
            level->gems.push_back(ent_gem);
        }

        for( auto& key : data["items"]["keys"] )
        {
            float x = key["position"]["x"];
            float y = key["position"]["y"];
            auto ent_key = new Entities::Items::Key({x, y});
            level->objects.push_back(ent_key->get_key_object());
            level->keys.push_back(ent_key);
        }

        if( levels.empty() )
            level->background = LoadTexture("assets\\backgrounds\\background.png");
        else
            level->background = levels[0]->background;
        std::cout << "Finished loading level \n";
        add_level(level);
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
        camera->target = level->spawn_point;
        Game::PAUSE = true;
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

    void GameWorld::start_next_level()
    {
        start_level(get_next_level());
    }

    Level* GameWorld::get_active_level() const
    {
        if(!active_level)
            return nullptr;
        return active_level;
    }

    Level* GameWorld::get_next_level() const
    {
            
        if( active_level->id < levels.size() - 1)
        {
            if( active_level->id == -1)
                return nullptr;
            return levels[active_level->id + 1];
        }
        return nullptr;
    }

    void GameWorld::open_level_complete_menu()
    {
        level_complete_menu->open();
    }

    void GameWorld::add_level(Level* level)
    {
        level->id = levels.size();
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
            if( active_level->player->get_hearts() <= 0 )
            {
                Game::PAUSE = true;
                start();
            }
            if( Game::PAUSE && IsKeyPressed(KEY_SPACE) )
            {
                Game::PAUSE = false;
            }
                
            if( IsKeyPressed(KEY_R))
            {
                active_level->player->reset_player();
                start();
            }
            active_level->player->update();
            if( active_level->camera_follows_player )
            {
                camera->target.x += (active_level->player->get_position().x - camera->target.x) * 0.01;
                camera->target.y = active_level->player->get_position().y;
            }
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
            for( auto& key : active_level->keys )
            {
                key->update();
            }
        }
        level_complete_menu->update();
    }

    void GameWorld::update_fixed()
    {
        level_complete_menu->update_fixed();
    }

    void GameWorld::draw_hearts(int hearts_remaining, int hearts_max)
    {
        int heart_width = 60;
        int offset = (GetScreenWidth() - hearts_max * heart_width) / 2;
        for( int i = 0; i < hearts_max; i++)
        {
            if( i < hearts_remaining )
                DrawTexture(heart_filled_texture, offset + (i * heart_width), 100, WHITE);
            else
                DrawTexture(heart_empty_texture, offset + (i * heart_width), 100, WHITE);
        }
    }

    unsigned char r = 255;
    unsigned char g = 255;
    unsigned char b = 255;
    void GameWorld::draw()
    {
        
        if( active_level )
        {
            if( active_level->background.id == 0)
                active_level->background = levels[0]->background;
            DrawTexture(active_level->background, -1500, -500, {r, g, b, 255});
            
            for( auto& obj : active_level->objects )
                if( obj->type == Physics::Shapes::WALL)
                    DrawLineEx(obj->start, obj->end, 4, {255, 255, 255, 255});
            for( auto& platform : active_level->platforms )
                platform->draw();
            for( auto& timed_platform : active_level->timed_platforms )
                timed_platform->draw();
            for( auto& false_platform : active_level->false_platforms )
                false_platform->draw();
            for( auto& spike : active_level->spikes )
                spike->draw();
            for( auto& gem : active_level->gems )
                gem->draw();
            for( auto& key : active_level->keys )
                key->draw();
            active_level->player->draw();
            if( active_level->finish_box )
                active_level->finish_box->draw();
        }
        EndMode2D();

        
        level_complete_menu->draw();
        if( active_level )
        {
            if(Game::PAUSE && active_level->id != -1)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 128});
                const char* press_to_start = "Press space to start";
                DrawText(press_to_start, GetScreenWidth() / 2 - MeasureText(press_to_start, 40) / 2, 400, 40, WHITE);
            }
            if( active_level->id != -1)
            {
                draw_hearts(active_level->player->get_hearts(), 3);
                const char* score_text = TextFormat("Score: %i", (int)active_level->player->get_score());
                int text_width = MeasureText(score_text, 48);
                DrawText(score_text, (GetScreenWidth() - text_width) / 2, 300, 48, WHITE);
            }

        }
        BeginMode2D(*camera);
    }
}
