#include "platform.h"

namespace Game::Entities::Obstacles
{
    Platform::Platform(Vector2 start, Vector2 end): Entity(start, 0)
    {
        position_end = end;
        platform = Physics::create_platform(start, end);
    }

    void Platform::update()
    {
        Entity::update();
    }

    void Platform::draw()
    {
        Entity::draw();
        DrawLineEx(position, position_end, 2, RED);
    }
}
