#include "platform.h"

namespace Game::Entities::Obstacles
{
    Platform::Platform(Vector2 start, Vector2 end): Entity(start, 0)
    {
        this->position = start;
        this->position_end = end;   
        platform = Physics::create_platform(position, position_end);
        Physics::ObjectDetails* details = new Physics::ObjectDetails();
        details->tag = Physics::ObjectDetails::GROUND;
        details->data = this;
        cpShapeSetUserData(platform->shape, details);
    }

    Vector2 Platform::get_end()
    {
        return position_end;
    }

    Physics::Object* Platform::get_platform()
    {
        return platform;
    }

    void Platform::update()
    {
        Entity::update();
    }

    void Platform::draw()
    {
        Entity::draw();
        DrawLineEx(position, position_end, 4, WHITE);

    }

    void Platform::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
    {
        Entity::on_collision(arb, space, entity);
    }
}
