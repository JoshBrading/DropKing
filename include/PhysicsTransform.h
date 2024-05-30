#pragma once
#include <corecrt_math.h>
#include <raylib.h>

namespace Physics
{
    struct Transform
    {
        float position_x;
        float position_y;
        float sin;
        float cos;

        Transform(Vector2 position, float angle)
        {
            this->position_x = position.x;
            this->position_y = position.y;
            this->sin = sinf(angle);
            this->cos = cosf(angle);
        }

        Transform(float x, float y, float angle)
        {
            this->position_x = x;
            this->position_y = y;
            this->sin = sinf(angle);
            this->cos = cosf(angle);
        }
    };

    inline Vector2 transform_vector(Vector2 v, Transform transform)
    {
        Vector2 transformed = { transform.cos * v.x - transform.sin * v.y + transform.position_x, 
                                transform.sin * v.x + transform.cos * v.y + transform.position_y };
        return transformed;
        
    }
}
