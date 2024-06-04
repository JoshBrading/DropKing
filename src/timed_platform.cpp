#include "timed_platform.h"

namespace Game::Entities::Obstacles
{
    TimedPlatform::TimedPlatform(Vector2 start, Vector2 end, int lifetime): Entity(start, 0)
    {
        this->position_end = end;
        this->lifetime = lifetime;
        this->platform = Physics::create_platform(start, end);
        Physics::ObjectDetails* details = new Physics::ObjectDetails();
        details->tag = Physics::ObjectDetails::GROUND;
        details->data = this;
        cpShapeSetUserData(platform->shape, details);
        this->tag = TIMED_PLATFORM;
    }

    Physics::Object* TimedPlatform::get_platform()
    {
        return platform;
    }

    void TimedPlatform::reset()
    {
        alive = true;
        collision_time = -1;
    }

    void TimedPlatform::update()
    {
        Entity::update();
        if( collision_time == -1) return;
        if( alive && lifetime < GetTime() - collision_time)
        {
            alive = false;
            Physics::remove_object_from_physics(platform);
        }
    }

    void TimedPlatform::draw()
    {
        Entity::draw();
        if( alive )
        {
            unsigned char alpha = collision_time != -1? 256 - (GetTime() - collision_time) * 255 / lifetime: 255;
            DrawLineEx(position, position_end, 2, {255, 255, 255, alpha });
        }
    }

    void TimedPlatform::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
    {
        Entity::on_collision(arb, space, entity);
        if( !entity ) return;
        if( entity->get_tag() == PLAYER )
        {
            collision_time = GetTime();
        }
    }
}
