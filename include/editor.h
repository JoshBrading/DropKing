#pragma once
#include <vector>

#include "menu.h"
#include "physics.h"

class Editor
{
    enum State
    {
        NONE,
        WAITING,
        EDITING,
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
    Editor(Camera2D* camera);
    void update();
    void update_fixed();
    void draw();
private:
    bool active = false;
    Camera2D* camera = nullptr;
    State state = NONE;
    Menu* menu = nullptr;
    EditObject* selected_object = nullptr;
    std::vector<Physics::Object*> objects;
};
