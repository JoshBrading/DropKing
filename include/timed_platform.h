#pragma once

#include "entity.h"
#include "physics.h"

namespace Game::Entities::Obstacles
{
    
    class TimedPlatform : public Entity
    {
        int collision_time = -1;
        int lifetime = -1;
        bool alive = true;
        Vector2 position_end;
        Physics::Object* platform;
        
    public:
        explicit TimedPlatform(Vector2 start, Vector2 end, int lifetime);
        Vector2 get_end();
        int get_lifetime();
        Physics::Object* get_platform();
        void reset();
        void update() override;
        void draw() override;
        void on_collision(cpArbiter* arb, cpSpace* space, Entity* entity) override;
    };
}
