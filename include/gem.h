#pragma once

#include "entity.h"

namespace Game::Entities::Items
{
    class Gem : public Entity
    {
        bool collected = false;
        Texture2D gem_texture;
        Physics::Object* gem_object;
    public:
        explicit Gem(Vector2 position);
        Physics::Object* get_gem_object();
        void reset();
        void update() override;
        void draw() override;
        void on_collision(cpArbiter* arb, cpSpace* space, Entity* entity) override;
    };
}