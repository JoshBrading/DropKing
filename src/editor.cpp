#include "editor.h"

#include <fstream>
#include <iostream>
#include <raymath.h>
#include <nlohmann/json.hpp>

#include "finish_box.h"
#include "gem.h"
#include "key.h"
#include "platform.h"
#include "spikes.h"

Editor::Editor(Camera2D* camera, Game::GameWorld* game)
{
    this->camera = camera;
    this->game = game;
    menu = new Menu();
    Font font = GetFontDefault();
    menu->add_label("Objects", font, 20, {5, 10});
    menu->add_button("Create Wall", font, {10, 35}, 200, 20, [this](Menu* menu, void* data)
    {
        this->state = PLATFORM_WAIT_FOR_CLICK;
        menu->close();
    }, nullptr);
    menu->add_label("Items", font, 20, {5, 65});
    menu->add_button("Create Gem", font, {10, 90}, 200, 20, [this](Menu* menu, void* data)
    {
        this->state = GEM_WAIT_FOR_PLACEMENT;
        menu->close();
    }, nullptr);
    menu->add_button("Create Key", font, {10, 115}, 200, 20, [this](Menu* menu, void* data)
    {
        this->state = KEY_WAIT_FOR_PLACEMENT;
        menu->close();
    }, nullptr);
    menu->add_button("Create Door", font, {10, 140}, 200, 20, [](Menu* menu, void* data)
    {
    }, nullptr);
    menu->add_label("Obstacles", font, 20, {5, 170});
    menu->add_button("Create Spike", font, {10, 195}, 200, 20, [this](Menu* menu, void* data)
    {
        this->state = SPIKE_PIT_WAIT_FOR_CLICK;
        menu->close();
    }, nullptr);
    menu->add_button("Create False Platform", font, {10, 220}, 200, 20, [this](Menu* menu, void* data)
    {
        this->state = FALSE_PLATFORM_WAIT_FOR_CLICK;
        menu->close();
    }, nullptr);
    menu->add_button("Create Timed Platform", font, {10, 245}, 200, 20, [this](Menu* menu, void* data)
    {
        this->state = TIMED_PLATFORM_WAIT_FOR_CLICK;
        menu->close();
    }, nullptr);
    menu->add_label("Level Data", font, 20, {5, 275});
    menu->add_button("Set Spawn Point", font, {10, 300}, 200, 20, [this](Menu* menu, void* data)
    {
        this->state = SETTING_SPAWN;
        menu->close();
    }, nullptr);
    menu->add_button("Finish Box", font, {10, 325}, 200, 20, [this](Menu* menu, void* data)
    {
        this->state = FINISH_BOX_WAIT_FOR_CLICK;
        menu->close();
    }, nullptr);
    menu->add_button("Demo Level", font, {10, 350}, 200, 20, [this, game](Menu* menu, void* data)
    {
        if( state != DEMO )
        {
            Game::PAUSE = false;
            state = DEMO;
            level->camera_follows_player = true;
            game->start();
        }
        else
        {
            Game::PAUSE = true;
            state = NONE;
            level->camera_follows_player = false;
            level->player->reset_player();
        }
        menu->close();
    }, nullptr)->is_toggle = true;
    menu->add_button("Save Level", font, {10, 375}, 200, 20, [this](Menu* menu, void* data)
    {
        this->save_level_to_file();
        menu->close();
    }, nullptr);
    menu->darken_background = false;


    for(auto& obj : objects)
    {
        Physics::add_object_to_physics(obj);
    }
}

void Editor::start()
{
    active = true;
    level = new Game::Level();
    level->objects.push_back(Physics::create_wall({-500, 0}, 10000));
    level->objects.push_back(Physics::create_wall({500, 0}, 10000));
    level->player = new Game::Entities::Player({0, 0});
    level->camera_follows_player = false;
    game->start_level(level);
}

void Editor::cleanup()
{
    active = false;
    level = nullptr;
    delete selected_object;
}

void Editor::update()
{
    if( !active ) return;
    Vector2 mouse_screen_space = GetMousePosition();
    Vector2 mouse = GetScreenToWorld2D(mouse_screen_space, *camera);
    if( IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
    {
        mouse.x = (mouse.x + (mouse.x >= 0 ? 6.25 : -6.25)) / 12.5 * 12.5;
        mouse.y = (mouse.y + 6.25) / 12.5 * 12.5;
    }
    else
    {
        mouse.x = ((int)(mouse.x + (mouse.x >= 0 ? 12.5 : -12.5)) / 25) * 25;
        mouse.y = (int)((mouse.y + 12.5) / 25) * 25;
    }
    
    if( IsMouseButtonPressed(1) && (state == NONE || state == DEMO))
    {
        menu->set_base_offset(mouse_screen_space);
        menu->open();
    }
    if( IsMouseButtonPressed(0))
    {
        if( state == SETTING_SPAWN )
        {
            level->spawn_point = mouse;
            level->player->set_spawn_point(level->spawn_point);
            level->player->reset_player();
            state = NONE;
        }
        if( state == PLATFORM_WAIT_FOR_CLICK )
        {
            selected_object = new EditObject();
            selected_object->start = mouse;
            state = PLATFORM_WAIT_FOR_PLACEMENT;
        }
        if( state == PLATFORM_WAIT_FOR_PLACEMENT )
        {
            if( !Vector2Equals(selected_object->start, mouse))
            {
                auto obj = new Game::Entities::Obstacles::Platform(selected_object->start, mouse);
                level->platforms.push_back(obj);
                level->objects.push_back(obj->get_platform());
                Physics::add_object_to_physics(obj->get_platform());
                selected_object = nullptr;
                state = NONE;
            }
        }
        if( state == TIMED_PLATFORM_WAIT_FOR_CLICK )
        {
            selected_object = new EditObject();
            selected_object->start = mouse;
            state = TIMED_PLATFORM_WAIT_FOR_PLACEMENT;
        }
        if( state == TIMED_PLATFORM_WAIT_FOR_PLACEMENT )
        {
            if( !Vector2Equals(selected_object->start, mouse))
            {
                auto obj = new Game::Entities::Obstacles::TimedPlatform(selected_object->start, mouse, 1);
                level->timed_platforms.push_back(obj);
                level->objects.push_back(obj->get_platform());
                Physics::add_object_to_physics(obj->get_platform());
                selected_object = nullptr;
                state = NONE;
            }
        }
        if( state == FALSE_PLATFORM_WAIT_FOR_CLICK )
        {
            selected_object = new EditObject();
            selected_object->start = mouse;
            state = FALSE_PLATFORM_WAIT_FOR_PLACEMENT;
        }
        if( state == FALSE_PLATFORM_WAIT_FOR_PLACEMENT )
        {
            if( !Vector2Equals(selected_object->start, mouse))
            {
                auto obj = new Game::Entities::Obstacles::Platform(selected_object->start, mouse);
                level->false_platforms.push_back(obj);
                selected_object = nullptr;
                state = NONE;
            }
        }
        if( state == FINISH_BOX_WAIT_FOR_CLICK )
        {
            selected_object = new EditObject();
            selected_object->start = mouse;
            state = FINISH_BOX_WAIT_FOR_PLACEMENT;
        }
        if( state == FINISH_BOX_WAIT_FOR_PLACEMENT )
        {
            if( !Vector2Equals(selected_object->start, mouse))
            {
                auto obj = new Game::Entities::FinishBox(selected_object->start, mouse, game);
                level->objects.push_back(obj->get_finish_box());
                Physics::add_object_to_physics(obj->get_finish_box());
                level->finish_box = obj;
                selected_object = nullptr;
                state = NONE;
            }
        }
        if( state == GEM_WAIT_FOR_PLACEMENT )
        {
            auto obj = new Game::Entities::Items::Gem(mouse);
            level->objects.push_back(obj->get_gem_object());
            Physics::add_object_to_physics(obj->get_gem_object());
            level->gems.push_back(obj);
            state = NONE;
        }
        if( state == KEY_WAIT_FOR_PLACEMENT )
        {
            auto obj = new Game::Entities::Items::Key(mouse);
            level->objects.push_back(obj->get_key_object());
            Physics::add_object_to_physics(obj->get_key_object());
            level->keys.push_back(obj);
            level->player->reset_player();
            state = NONE;
        }
        if( state == SPIKE_PIT_WAIT_FOR_CLICK )
        {
            selected_object = new EditObject();
            selected_object->start = mouse;
            state = SPIKE_PIT_WAIT_FOR_PLACEMENT;
        }
        if( state == SPIKE_PIT_WAIT_FOR_PLACEMENT )
        {
            if( !Vector2Equals(selected_object->start, mouse))
            {
                auto obj = new Game::Entities::Obstacles::Spikes(selected_object->start, mouse);
                level->objects.push_back(obj->get_spikes());
                Physics::add_object_to_physics(obj->get_spikes());
                level->spikes.push_back(obj);
                selected_object = nullptr;
                state = NONE;
            }
        }
    }

    menu->update();
    if( IsMouseButtonPressed(0))
        menu->close();
}

void Editor::update_fixed() const
{
    if( !active ) return;
    
    menu->update_fixed();
    camera->target.y -= GetMouseWheelMove() * 100;
    if( IsKeyDown(KEY_W))
        camera->target.y -= 10;
    if( IsKeyDown(KEY_A))
        camera->target.x -= 10;
    if( IsKeyDown(KEY_S))
        camera->target.y += 10;
    if( IsKeyDown(KEY_D))
        camera->target.x += 10;
    
    
}

void Editor::draw() const
{
    if( !active ) return;
    
    BeginMode2D(*camera);
    if( level->finish_box )
        level->finish_box->draw();
    for(auto& obj : objects)
    {
        DrawLineEx(obj->start, obj->end, 4, WHITE);
    }
    for(auto& platform : level->platforms)
    {
        platform->draw();
    }
    for(auto& timed_platform : level->timed_platforms)
    {
        timed_platform->draw();
    }
    for(auto& false_platform : level->false_platforms)
    {
        false_platform->draw();
    }
    for(auto& gem : level->gems)
    {
        gem->draw();
    }
    for(auto& key : level->keys)
    {
        key->draw();
    }
    for(auto& spike : level->spikes)
    {
        spike->draw();
    }
    Vector2 mouse_screen_pos = GetMousePosition();
    Vector2 mouse = GetScreenToWorld2D(mouse_screen_pos, *camera);
    float grid_space = 25;
    if( IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))
    {
        mouse.x = (mouse.x + (mouse.x >= 0 ? 6.25 : -6.25)) / 12.5 * 12.5;
        mouse.y = (mouse.y + 6.25) / 12.5 * 12.5;
        grid_space = 12.5;
    }
    else
    {
        mouse.x = ((int)(mouse.x + (mouse.x >= 0 ? 12.5 : -12.5)) / 25) * 25;
        mouse.y = (int)((mouse.y + 12.5) / 25) * 25;
    }

    
    if( selected_object && state != FINISH_BOX_WAIT_FOR_PLACEMENT && state != SPIKE_PIT_WAIT_FOR_PLACEMENT)
        DrawLineEx(selected_object->start, mouse, 4, GREEN);
    else if( selected_object && (state == FINISH_BOX_WAIT_FOR_PLACEMENT || state == SPIKE_PIT_WAIT_FOR_PLACEMENT))
    {
        Rectangle rect = {selected_object->start.x, selected_object->start.y, mouse.x - selected_object->start.x, mouse.y - selected_object->start.y};
        DrawRectangleLinesEx(rect, 4, GREEN);
    }
    if( state != DEMO)
    {
        // Edit grid
        for( float i = -25; i < 10000; i += grid_space)
        {
            DrawLineEx({-575, (float)i}, {575, (float)i}, 1, {255, 255, 255, 64});
        }
        for( float i = -525; i <= 525; i += grid_space)
        {
            DrawLineEx({(float)i, -75}, {(float)i, 10000}, 1, {255, 255, 255, 64});
        }
        
        if( state != NONE )
            DrawCircle(mouse.x, mouse.y, 5, GREEN);
        
    }
    EndMode2D();
    const char* text;
    int text_length;
    if( state == PLATFORM_WAIT_FOR_CLICK )
    {
        text = "Click to set start point";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == PLATFORM_WAIT_FOR_PLACEMENT )
    {
        text = "Click to set end point";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == TIMED_PLATFORM_WAIT_FOR_CLICK )
    {
        text = "Click to set start point";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == TIMED_PLATFORM_WAIT_FOR_PLACEMENT )
    {
        text = "Click to set end point";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == SETTING_SPAWN )
    {
        text = "Click to set spawn point";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == FINISH_BOX_WAIT_FOR_CLICK )
    {
        text = "Click to set start point";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == FINISH_BOX_WAIT_FOR_PLACEMENT )
    {
        text = "Click to set end point";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == GEM_WAIT_FOR_PLACEMENT )
    {
        text = "Click to place gem";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == KEY_WAIT_FOR_PLACEMENT )
    {
        text = "Click to place key";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == SPIKE_PIT_WAIT_FOR_CLICK )
    {
        text = "Click to set start point";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }
    if( state == SPIKE_PIT_WAIT_FOR_PLACEMENT )
    {
        text = "Click to set end point";
        text_length = MeasureText(text, 20);
        DrawText(text, mouse_screen_pos.x - text_length / 2, mouse_screen_pos.y + 20, 20, SKYBLUE);
    }

    
    if( menu )
    {
        menu->draw();
    }
    
}

void Editor::save_level_to_file() const
{
    int level_count = game->get_levels().size();
    std::string file_name = "Level_" + std::to_string(level_count + 1);
    std::ofstream file("assets\\levels\\" + file_name + ".json");
    if( !file.is_open() )
    {
        std::cerr << "Failed to open file \n";
        return;
    }
    
    nlohmann::json data;
    data["name"] = "Test Level";
    data["width"] = 1000;
    data["background"] = "background.png";
    data["spawn_point"]["x"] = level->spawn_point.x;
    data["spawn_point"]["y"] = level->spawn_point.y;
    data["finish_box"]["start"]["x"] = level->finish_box->get_position().x;
    data["finish_box"]["start"]["y"] = level->finish_box->get_position().y;
    data["finish_box"]["end"]["x"] = level->finish_box->get_end().x;
    data["finish_box"]["end"]["y"] = level->finish_box->get_end().y;
    data["time_to_complete"] = 100;
    data["map_objects"]["walls"] = nlohmann::json::array();
    data["map_objects"]["platforms"] = nlohmann::json::array();
    data["obstacles"]["spikes"] = nlohmann::json::array();
    data["obstacles"]["false_platforms"] = nlohmann::json::array();
    data["obstacles"]["timed_platforms"] = nlohmann::json::array();
    data["items"]["gems"] = nlohmann::json::array();
    data["items"]["keys"] = nlohmann::json::array();
    data["items"]["doors"] = nlohmann::json::array();
    

    for( auto& platform : level->platforms )
    {
        nlohmann::json platform_json;
        platform_json["start"]["x"] = platform->get_position().x;
        platform_json["start"]["y"] = platform->get_position().y;
        platform_json["end"]["x"] = platform->get_end().x;
        platform_json["end"]["y"] = platform->get_end().y;
        platform_json["length"] = platform->get_end().x;
        platform_json["angle"] = 0;
        data["map_objects"]["platforms"].push_back(platform_json);
    }
    
    for( auto& timed_platform : level->timed_platforms )
    {
        nlohmann::json platform_json;
        platform_json["start"]["x"] = timed_platform->get_position().x;
        platform_json["start"]["y"] = timed_platform->get_position().y;
        platform_json["end"]["x"] = timed_platform->get_end().x;
        platform_json["end"]["y"] = timed_platform->get_end().y;
        platform_json["time"] = timed_platform->get_lifetime();
        platform_json["angle"] = 0;
        data["obstacles"]["timed_platforms"].push_back(platform_json);
    }

    for( auto& false_platform : level->false_platforms )
    {
        nlohmann::json platform_json;
        platform_json["start"]["x"] = false_platform->get_position().x;
        platform_json["start"]["y"] = false_platform->get_position().y;
        platform_json["end"]["x"] = false_platform->get_end().x;
        platform_json["end"]["y"] = false_platform->get_end().y;
        platform_json["length"] = false_platform->get_end().x;
        platform_json["angle"] = 0;
        data["obstacles"]["false_platforms"].push_back(platform_json);
    }

    for( auto& gem : level->gems )
    {
        nlohmann::json gem_json;
        gem_json["position"]["x"] = gem->get_position().x;
        gem_json["position"]["y"] = gem->get_position().y;
        data["items"]["gems"].push_back(gem_json);
    }
    file << data.dump(4);

    game->add_level(level);
}
