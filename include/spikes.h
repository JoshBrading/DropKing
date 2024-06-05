#pragma once

#include "entity.h"

namespace Game::Entities::Obstacles
{
    
    class Spikes : public Entity
    {
        Vector2 position_end;
        Texture2D spikes_texture;
        Physics::Object* spikes;
        
    public:
        explicit Spikes(Vector2 start, Vector2 end);
        Vector2 get_end();
        Physics::Object* get_spikes();
        void update() override;
        void draw() override;
        void on_collision(cpArbiter* arb, cpSpace* space, Entity* entity) override;
    };
}