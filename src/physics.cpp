#include "physics.h"

#include <complex>
#include <raylib.h>

#include "entity.h"
#include "transform.h"


void chipmunk_post_step(cpSpace* space, void* key, void* data)
{
    
}

void chipmunk_pre_solve_collision(cpArbiter* arb, cpSpace* space, cpDataPointer data)
{
    (void)space;
    (void)data;
    if (cpArbiterIsFirstContact(arb))
    {
        cpShape *a, *b;
        cpArbiterGetShapes(arb, &a, &b);
        Physics::ObjectDetails* details_a = static_cast<Physics::ObjectDetails*>(cpShapeGetUserData(a));
        Physics::ObjectDetails* details_b = static_cast<Physics::ObjectDetails*>(cpShapeGetUserData(b));
        Entity* entity_a = nullptr;
        Entity* entity_b = nullptr;

        if( details_a )
            entity_a = static_cast<Entity*>(details_a->data);
        if( details_b )
            entity_b = static_cast<Entity*>(details_b->data);
        
        if( entity_a )
            entity_a->on_collision(arb, space, entity_b);
        if( entity_b )
            entity_b->on_collision(arb, space, entity_a);
        
        cpSpaceAddPostStepCallback(space, chipmunk_post_step, NULL, NULL);
    }
}



namespace Physics
{
    Physics::Object* create_square(Vector2 position, Vector2 size)
    {
        Object* obj = new Object();
        obj->type = Shapes::BOX;
        obj->size.x = size.x;
        obj->size.y = size.y;
        float mass = (obj->size.x * obj->size.y) * 0.001f;
        obj->body = cpBodyNew(mass, cpMomentForBox(mass, obj->size.x, obj->size.y));
        obj->shape = cpBoxShapeNew(obj->body, obj->size.x, obj->size.y, 0.0);
        return obj;
    }

    Physics::Object* create_static_square( Vector2 position, Vector2 size )
    {
        Object* obj = new Object();
        obj->type = Shapes::BOX;
        obj->size.x = size.x;
        obj->size.y = size.y;
        obj->start = position;
        obj->end = {position.x + size.x, position.y + size.y};
        obj->body = cpBodyNewStatic();
        cpBodySetPosition(obj->body, {position.x, position.y});
        obj->shape = cpBoxShapeNew(obj->body, obj->size.x, obj->size.y, 0.0);
        return obj;
    }

    Object* create_platform(Vector2 start, float length, float deg)
    {
        Object* obj = new Object();
        obj->type = Shapes::PLATFORM;
        cpBody* body = cpSpaceGetStaticBody(Instances::SPACE);
        Vector2 end = transform::rotate_point_about_target(start, {start.x, start.y - length}, deg);
        cpShape* shape = cpSegmentShapeNew(body, cpv(start.x, start.y), cpv(end.x, end.y), 0);
        obj->body = body;
        obj->shape = shape;
        obj->start = start;
        obj->end = end;
        return obj;
    }
    
    Object* create_platform(Vector2 start, Vector2 end)
    {
        Object* obj = new Object();
        obj->type = Shapes::PLATFORM;
        cpBody* body = cpSpaceGetStaticBody(Instances::SPACE);
        cpShape* shape = cpSegmentShapeNew(body, cpv(start.x, start.y), cpv(end.x, end.y), 0);
        obj->body = body;
        obj->shape = shape;
        obj->start = start;
        obj->end = end;
        return obj;
    }
    
    Object* create_wall(Vector2 start, float height)
    {
        Object* obj = new Object();
        obj->type = Shapes::WALL;
        cpBody* body = cpSpaceGetStaticBody(Instances::SPACE);
        Vector2 end = {start.x, start.y + height};
        cpShape* shape = cpSegmentShapeNew(body, cpv(start.x, start.y), cpv(end.x, end.y), 0);
        ObjectDetails* wall = new ObjectDetails();
        wall->tag = ObjectDetails::WALL;
        cpShapeSetUserData(shape, wall);
        obj->body = body;
        obj->shape = shape;
        obj->start = start;
        obj->end = end;
        return obj;
    }

    void add_object_to_physics(Object* obj)
    {
        if( !obj ) return;
        
        if( !cpSpaceContainsBody(Instances::SPACE, obj->body))
            cpSpaceAddBody(Instances::SPACE, obj->body);
        
        cpSpaceAddShape(Instances::SPACE, obj->shape);
    }

    void remove_object_from_physics(Object* obj)
    {
        if(!obj) return;
        
        if( obj->body != cpSpaceGetStaticBody(Instances::SPACE) )
            cpSpaceRemoveBody(Instances::SPACE, obj->body);
        
        cpSpaceRemoveShape(Instances::SPACE, obj->shape);
    }

    void check_grounded(cpBody* body, cpArbiter* arb, void* obj)
    {
        Object* object = (Object*)obj;
        if( !object ) return;
        
        cpShape *a, *b;
        cpArbiterGetShapes(arb, &a, &b);
        if( object->shape != a && object->shape != b) return;

        ObjectDetails* details = (ObjectDetails*)cpShapeGetUserData(b);
        if( !details) return;
        
        cpVect n = cpArbiterGetNormal(arb);
        cpArbiterSetRestitution(arb, 0.0f);
        
        if( details->tag == ObjectDetails::GROUND)
        {
            object->is_grounded = true;
            object->ground_normal = n;
            // object->ground = b;
        }
        
    }

    void init()
    {
        Instances::SPACE = cpSpaceNew();
        Instances::COLLISION_HANDLER = cpSpaceAddCollisionHandler(Instances::SPACE, 0, 0);
        Instances::COLLISION_HANDLER->preSolveFunc = reinterpret_cast<cpCollisionPreSolveFunc>(chipmunk_pre_solve_collision);
    }
}
