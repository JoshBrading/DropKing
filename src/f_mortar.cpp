#include "f_mortar.h"
#include <raymath.h>
#include "Projectile.h"

FMortar::FMortar(const Vector3 position): Entity(position, "assets/projectile.obj", "Mortar")
{
}

void FMortar::draw()
{
    Entity::draw();
}

float MOVE_SPEED = 25.0f;
int NEW_TARGET_TIMER = 0;
float TOLERANCE = 0.02f;
void FMortar::update()
{
    if (NEW_TARGET_TIMER <= GetTime())
    {
        // Set new random target position and reset timer
        const Vector3 test = {static_cast<float>(GetRandomValue(-10, 10)), static_cast<float>(GetRandomValue(0, 10)), static_cast<float>(GetRandomValue(-10, 20))};
        set_target_position(test); // Change range as needed
        NEW_TARGET_TIMER = static_cast<int>(GetTime()) + 200; // Reset timer to 10 seconds (assuming 60 frames per second)
        
    }
    else
    {
        if(!Vector3Equals(position, get_target_position()))
        {

            // Move towards the target position
            const Vector3 direction = Vector3Normalize(Vector3Subtract(get_target_position(), position));
            position = Vector3Add(position, Vector3Scale(direction, MOVE_SPEED * GetFrameTime()));
        }
        NEW_TARGET_TIMER--;
    }
    
    Entity::update();
    if( IsKeyDown(KEY_W) )
        position.z -= MOVE_SPEED * GetFrameTime();
    if( IsKeyDown(KEY_A) )
        position.x -= MOVE_SPEED * GetFrameTime();
    if( IsKeyDown(KEY_S) )
        position.z += MOVE_SPEED * GetFrameTime();
    if( IsKeyDown(KEY_D) )
        position.x += MOVE_SPEED * GetFrameTime();
}

