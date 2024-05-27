#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <raylib.h>

namespace transform
{
    Vector2 rotate_point_about_target(const Vector2 origin, Vector2 point, const float angle);
}

#endif // TRANSFORM_H