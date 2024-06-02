#include "editor.h"

#include <raymath.h>

Editor::Editor(Camera2D* camera)
{
    this->camera = camera;
    menu = new Menu();
    Font font = GetFontDefault();
    menu->add_label("Editor", font, 20, {10, 10});
    menu->add_button("Create Wall", font, {10, 40}, 200, 20, [](Menu* menu, void* data)
    {
        Editor* editor = static_cast<Editor*>(data);
        editor->state = WAITING;
    }, this);
    menu->darken_background = false;
    menu->toggle();
}

void Editor::update()
{
    
    Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), *camera);
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
    
    if( IsKeyDown(KEY_W))
        camera->target.y -= 10;
    if( IsKeyDown(KEY_A))
        camera->target.x -= 10;
    if( IsKeyDown(KEY_S))
        camera->target.y += 10;
    if( IsKeyDown(KEY_D))
        camera->target.x += 10;
}

void Editor::draw()
{
    if( menu )
    {
        menu->draw();
        menu->update();
    }
    BeginMode2D(*camera);
    for(auto& obj : objects)
    {
        DrawLineEx(obj->start, obj->end, 4, WHITE);
    }
    Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), *camera);
    if( selected_object )
        DrawLineEx(selected_object->start, mouse, 4, GREEN);
    EndMode2D();
}
