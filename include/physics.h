#pragma once

#include <raylib.h>
#include <chipmunk/chipmunk.h>

namespace Physics
{
    inline int MAX_OBJECTS = 12;


    namespace Instances
    {
        inline cpSpace* SPACE = cpSpaceNew();
    }
    namespace Shapes
    {
        enum ShapeType
        {
            NONE,
            CIRCLE,
            BOX
        };
    }

    struct Object
    {
        Shapes::ShapeType type = Shapes::NONE;
        cpBody* body = nullptr;
        cpShape* shape = nullptr;
        Vector2 size = {};

        void update();
    };

    class World
    {
        
    };

    
    Object create_square( Vector2 position, Vector2 size);
}
