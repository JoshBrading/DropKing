#include "transform.h"
#include <raylib.h>
#include <raymath.h>

Vector2 transform::rotate_point_about_target(const Vector2 origin, Vector2 point, const float angle) {
    const float radians = angle * PI / 180.0f;

    const float s = sinf(radians);
    const float c = cosf(radians);

    point.x -= origin.x;
    point.y -= origin.y;

    const Vector2 rotation = {point.x * c - point.y * s, point.x * s + point.y * c};
    const Vector2 rotated_point = Vector2Add(rotation, origin);

    return rotated_point;
}
