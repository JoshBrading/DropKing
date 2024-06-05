#pragma once

#include <raylib.h>
#include <chipmunk/chipmunk.h>

namespace Physics
{
    inline int MAX_OBJECTS = 1;
    inline cpVect GRAVITY = cpv(0, 1000);

    namespace Instances
    {
        inline cpSpace* SPACE;
        inline cpCollisionHandler* COLLISION_HANDLER;
    }
    namespace Shapes
    {
        enum ShapeType
        {
            NONE,
            CIRCLE,
            BOX,
            PLATFORM,
            WALL
        };
    }

    struct ObjectDetails
    {
        enum Tag
        {
            NONE,
            GROUND,
            WALL,
            PLAYER,
            FINISH
        };
        
        Tag tag;
        void* data;
    };

    struct Object
    {
        Shapes::ShapeType type = Shapes::NONE;
        cpBody* body = nullptr;
        cpShape* shape = nullptr;
        Vector2 size = {};
        Vector2 start = {};
        Vector2 end = {};
        bool is_grounded = false;
        bool is_jumping = false;
        cpVect ground_normal = cpv(0, 0);
        cpBody* ground = nullptr;
        float max_jump_height = 1000;
        float jump_scale = 0.0f;
        float max_jump_scale = 2.0f;
        float jump_scale_increase = 2.0f;
    };
    void init();
    Object* create_square(Vector2 position, Vector2 size);
    Object* create_static_square(Vector2 position, Vector2 size);
    Object* create_platform(Vector2 start, float length, float deg);
    Object* create_platform(Vector2 start, Vector2 end);

    Object* create_wall(Vector2 start, float height);
    void add_object_to_physics(Object* obj);
    void remove_object_from_physics(Object* obj);
}
