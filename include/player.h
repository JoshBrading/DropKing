#pragma once

#include "entity.h"
#include "physics.h"


namespace Game::Entities
{
    class Player : public Entity
    {
        Physics::Object* player_object;
        Vector2 spawn_point;
    public:
        explicit Player(Vector2 position);
        void reset_player();
        void set_spawn_point(Vector2 spawn_point);
        Physics::Object* get_player_object();
        void update() override;
        void draw() override;
        void on_collision(cpArbiter* arb, cpSpace* space, Entity* entity) override;
    };
}
