#pragma once

#include <raylib.h>
#include <chipmunk/chipmunk.h>

namespace Physics
{
    inline int MAX_OBJECTS = 1;
    inline cpVect GRAVITY = cpv(0, 1000);

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

    struct ObjectDetails
    {
        enum Tag
        {
            NONE,
            GROUND,
            WALL
        };
        Tag tag;
    };

    struct Object
    {
        Shapes::ShapeType type = Shapes::NONE;
        cpBody* body = nullptr;
        cpShape* shape = nullptr;
        Vector2 size = {};
        bool is_grounded = false;
        bool is_jumping = false;
        cpVect ground_normal = cpv(0, 0);
        cpBody* ground = nullptr;
        float max_jump_height = 1000;
        float jump_scale = 0.0f;
        float max_jump_scale = 2.0f;
        float jump_scale_increase = 2.0f;
        void update();
    };

    class World
    {
        
    };


    Physics::Object* create_square(Vector2 position, Vector2 size);
}
