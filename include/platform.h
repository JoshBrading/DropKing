#pragma once

#include "entity.h"
#include "physics.h"

namespace Game::Entities::Obstacles
{
    
    class Platform : public Entity
    {
        Physics::Object* platform;
        Vector2 position_end;
        
    public:
        explicit Platform(Vector2 start, Vector2 end);
        void update() override;
        void draw() override;
    
    };
}
