#ifndef COLLISION_H
#define COLLISION_H
#include <vector>
#include <raylib.h>

struct Polygon
{
    Vector2 origin;
    std::vector<Vector2> points;
};

struct Circle
{
    Vector2 origin;
    float radius;
};

bool line_intersects_line( Vector2 a1, Vector2 a2, Vector2 b1, Vector2 b2 );

bool polygon_intersects_polygon(const Polygon& p1, const Polygon& p2, Vector2 &normal, float &depth );

bool polygon_intersects_circle(const Polygon& p, const Circle& c, Vector2& normal, float& depth);

#endif // COLLISION_H
