#include "editor.h"

#include <fstream>
#include <iostream>
#include <raymath.h>
#include <nlohmann/json.hpp>

Editor::Editor(Camera2D* camera)
{
    this->camera = camera;
    menu = new Menu();
    Font font = GetFontDefault();
    menu->add_label("Objects", font, 20, {5, 10});
    menu->add_button("Create Wall", font, {10, 35}, 200, 20, [](Menu* menu, void* data)
    {
        Editor* editor = static_cast<Editor*>(data);
        editor->state = WAITING;
        menu->close();
    }, this);
    menu->add_label("Items", font, 20, {5, 65});
    menu->add_button("Create Gem", font, {10, 90}, 200, 20, [](Menu* menu, void* data)
    {
    }, this);
    menu->add_button("Create Key", font, {10, 115}, 200, 20, [](Menu* menu, void* data)
    {
    }, this);
    menu->add_button("Create Door", font, {10, 140}, 200, 20, [](Menu* menu, void* data)
    {
    }, this);
    menu->add_label("Obstacles", font, 20, {5, 170});
    menu->add_button("Create Spike", font, {10, 195}, 200, 20, [](Menu* menu, void* data)
    {
    }, this);
    menu->add_button("Create False Platform", font, {10, 220}, 200, 20, [](Menu* menu, void* data)
    {
    }, this);
    menu->add_button("Create Timed Platform", font, {10, 245}, 200, 20, [](Menu* menu, void* data)
    {
    }, this);
    menu->add_label("Save", font, 20, {5, 275});
    menu->add_button("Save Level", font, {10, 300}, 200, 20, [](Menu* menu, void* data)
    {
        Editor* editor = static_cast<Editor*>(data);
        editor->save_level_to_file();
        menu->close();
    }, this);
    menu->darken_background = false;

    objects.push_back(Physics::create_wall({-500, 0}, 10000));
    objects.push_back(Physics::create_wall({500, 0}, 10000));

    for(auto& obj : objects)
    {
        Physics::add_object_to_physics(obj);
    }

    
    //save_level_to_file();
    
}

void Editor::update()
{
    Vector2 mouse_screen_space = GetMousePosition();
    Vector2 mouse = GetScreenToWorld2D(mouse_screen_space, *camera);
    if( IsMouseButtonPressed(1))
    {
        menu->set_base_offset(mouse_screen_space);
        menu->open();
    }
    if( state == WAITING )
    {
        if( IsMouseButtonPressed(0))
        {
            selected_object = new EditObject();
            selected_object->start = mouse;
            state = EDITING;
        }
    }
    if( state == EDITING )
    {
        if( !Vector2Equals(selected_object->start, mouse) && IsMouseButtonPressed(0))
        {
            Physics::Object* obj = Physics::create_platform(selected_object->start, mouse);
            objects.push_back(obj);
            Physics::add_object_to_physics(obj);
            selected_object = nullptr;
            state = NONE;
        }
    }
    
}

void Editor::update_fixed()
{
    menu->update_fixed();
    camera->offset.y += GetMouseWheelMove()*50;
    
}

void Editor::draw()
{
    BeginMode2D(*camera);
    for(auto& obj : objects)
    {
        DrawLineEx(obj->start, obj->end, 4, WHITE);
    }
    Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), *camera);
    if( selected_object )
        DrawLineEx(selected_object->start, mouse, 4, GREEN);
    EndMode2D();
    
    if( menu )
    {
        menu->draw();
        menu->update();
    }
}

void Editor::save_level_to_file()
{
    std::ofstream file("assets\\levels\\level_1.json");
    if( !file.is_open() )
    {
        std::cerr << "Failed to open file \n";
        return;
    }
    
    nlohmann::json data;
    data["name"] = "Test Level";
    data["width"] = 1000;
    data["background"] = "background.png";
    data["spawn_point"]["x"] = 0;
    data["spawn_point"]["y"] = 0;
    data["end_height"] = 1000;
    data["time_to_complete"] = 100;
    data["map_objects"]["walls"] = nlohmann::json::array();
    data["map_objects"]["platforms"] = nlohmann::json::array();
    data["obstacles"]["spikes"] = nlohmann::json::array();
    data["obstacles"]["false_platforms"] = nlohmann::json::array();
    data["obstacles"]["timed_platforms"] = nlohmann::json::array();
    data["items"]["gems"] = nlohmann::json::array();
    data["items"]["keys"] = nlohmann::json::array();
    data["items"]["doors"] = nlohmann::json::array();
    for(auto& obj : objects)
    {
        if( obj->type == Physics::Shapes::WALL )
        {
            nlohmann::json wall;
            wall["start"]["x"] = obj->start.x;
            wall["start"]["y"] = obj->start.y;
            wall["end"]["x"] = obj->end.x;
            wall["end"]["y"] = obj->end.y;
            wall["height"] = obj->end.y;
            data["map_objects"]["walls"].push_back(wall);
        }
        else if( obj->type == Physics::Shapes::PLATFORM )
        {
            nlohmann::json platform;
            platform["start"]["x"] = obj->start.x;
            platform["start"]["y"] = obj->start.y;
            platform["end"]["x"] = obj->end.x;
            platform["end"]["y"] = obj->end.y;
            platform["length"] = obj->end.x;
            platform["angle"] = 0;
            data["map_objects"]["platforms"].push_back(platform);
        }
    }
    file << data.dump(4);
}
