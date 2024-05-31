#include "physics.h"

#include <raylib.h>

namespace Physics
{
    Object create_square( Vector2 position, Vector2 size)
    {
        Object obj;
        obj.type = Physics::Shapes::BOX;
        obj.size.x = size.x;
        obj.size.y = size.y;
        float mass = (obj.size.x * obj.size.y) * 0.001;
        obj.body = cpSpaceAddBody(Physics::Instances::SPACE, cpBodyNew(mass, cpMomentForBox(mass, obj.size.x, obj.size.y)));
        obj.shape = cpSpaceAddShape(Physics::Instances::SPACE, cpBoxShapeNew(obj.body, obj.size.x, obj.size.y, 0.0));
        return obj;
    }
}
