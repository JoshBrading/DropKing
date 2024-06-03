#pragma once
#include <vector>

#include "game.h"
#include "menu.h"
#include "physics.h"

class Editor
{
    enum State
    {
        NONE,
        WAITING,
        EDITING,
        SETTING_SPAWN,
        DEMO,
    };
    struct EditObject
    {
        Physics::Object* object;
        Vector2 start;
        Vector2 end;
        Vector2 size;
        float length;
    };
public:
    Editor(Camera2D* camera, Game::GameWorld* game);
    void start();
    void cleanup();
    void update();
    void update_fixed();
    void draw();
private:
    Camera2D* camera = nullptr;
    Game::GameWorld* game = nullptr;
    Game::Level* level = nullptr;
    
    bool active = false;
    State state = NONE;
    Menu* menu = nullptr;
    Vector2 spawn_point = {0, 0};
    EditObject* selected_object = nullptr;
    std::vector<Physics::Object*> objects;

    void save_level_to_file();
};
