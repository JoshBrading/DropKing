#pragma once

#include "entity.h"

namespace Game::Entities::Items
{
    class Key : public Entity
    {
        bool collected = false;
        Texture2D key_texture;
        Physics::Object* key_object;
        int multiplier = 3;
    public:
        explicit Key(Vector2 position);
        Physics::Object* get_key_object();
        void reset();
        void update() override;
        void draw() override;
        void on_collision(cpArbiter* arb, cpSpace* space, Entity* entity) override;
    };
}
