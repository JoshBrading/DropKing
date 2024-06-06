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
        DEMO,
        SETTING_SPAWN,
        FINISH_BOX_WAIT_FOR_CLICK,
        FINISH_BOX_WAIT_FOR_PLACEMENT,
        PLATFORM_WAIT_FOR_CLICK,
        PLATFORM_WAIT_FOR_PLACEMENT,
        TIMED_PLATFORM_WAIT_FOR_CLICK,
        TIMED_PLATFORM_WAIT_FOR_PLACEMENT,
        FALSE_PLATFORM_WAIT_FOR_CLICK,
        FALSE_PLATFORM_WAIT_FOR_PLACEMENT,
        SPIKE_PIT_WAIT_FOR_CLICK,
        SPIKE_PIT_WAIT_FOR_PLACEMENT,
        GEM_WAIT_FOR_PLACEMENT,
        KEY_WAIT_FOR_PLACEMENT,
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
    void update_fixed() const;
    void draw() const;
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

    void save_level_to_file() const;
};
