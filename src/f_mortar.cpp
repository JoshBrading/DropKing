#include "f_mortar.h"

#include <iostream>
#include <raymath.h>
#include "Projectile.h"

FMortar::FMortar(const Vector3 position): Entity(position, {0,0,0}, "assets/projectile.obj", "Mortar")
{
}

void FMortar::draw()
{
    Entity::draw();
}

float MOVE_SPEED = 20.0f;
float TOLERANCE = 0.02f;

void get_new_target(FMortar *mortar)
{
    const Vector3 target = {static_cast<float>(GetRandomValue(-10, 10)), static_cast<float>(GetRandomValue(0, 10)), static_cast<float>(GetRandomValue(-10, 20))};
    mortar->set_target_position(target);
}

void FMortar::update()
{
    Entity::update();
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        const Model proj_model = model;
        auto projectile = new Projectile(position, {10, 30, 0}, proj_model);
    }
    const float distance = Vector3Distance(position, get_target_position());
    if(distance > TOLERANCE)
    {
        const Vector3 direction = Vector3Normalize(Vector3Subtract(get_target_position(), position));
        position = Vector3Add(position, Vector3Scale(direction, MOVE_SPEED * GetFrameTime()));
    }
    else
    {
        get_new_target(this);
    }
}

