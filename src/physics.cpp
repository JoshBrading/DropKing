#include "physics.h"

#include <complex>
#include <raylib.h>

namespace Physics
{
    Physics::Object* create_square(Vector2 position, Vector2 size)
    {
        Object* obj = new Object();
        obj->type = Physics::Shapes::BOX;
        obj->size.x = size.x;
        obj->size.y = size.y;
        float mass = (obj->size.x * obj->size.y) * 0.001f;
        obj->body = cpSpaceAddBody(Physics::Instances::SPACE, cpBodyNew(mass, cpMomentForBox(mass, obj->size.x, obj->size.y)));
        obj->shape = cpSpaceAddShape(Physics::Instances::SPACE, cpBoxShapeNew(obj->body, obj->size.x, obj->size.y, 0.0));
        return obj;
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
    void Object::update()
    {
        //jump_scale = jump_scale + 1;
        DrawText(TextFormat("Jump: %f", jump_scale), 10, 30, 20, WHITE);
        cpVect curr_vel = cpBodyGetVelocity(body);
        cpBodyEachArbiter(body, check_grounded, this);
        if( is_grounded )
        {
            
            if( IsKeyDown(KEY_SPACE))
            {
                if( jump_scale < max_jump_scale)
                    jump_scale += jump_scale_increase * GetFrameTime();
            }
            else if( IsKeyReleased(KEY_SPACE))
            {
                cpVect jump_impulse = cpv(0, -(max_jump_height + max_jump_height * jump_scale));
                cpBodyApplyImpulseAtWorldPoint(body, jump_impulse, cpBodyGetPosition(body));
                cpBodySetAngularVelocity(body, 100 * PI / 180);
                is_grounded = false;
                jump_scale = 0.0f;
            }
        }
        
    }


}
