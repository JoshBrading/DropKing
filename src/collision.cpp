#include "collision.h"
#include <raymath.h>

float TOLERANCE = 0.01f;

void project_polygon_to_axis(const Vector2 axis, const Polygon& p, float &min, float &max)
{
    min = INFINITY;
    max = -INFINITY;

    for( const auto& point: p.points )
    {
        float projection = Vector2DotProduct(point, axis);

        if( projection < min )
            min = projection;
        if( projection > max )
            max = projection;
    }
}

void project_circle_to_axis(const Vector2 axis, const Circle& c, float &min, float &max )
{
    float projection = Vector2DotProduct(c.origin, axis);
    min = projection - c.radius;
    max = projection + c.radius;

    if( min > max )
        std::swap(min, max);
}

int closest_point_to_target(const std::vector<Vector2>& points, const Vector2 target )
{
    float min_distance = INFINITY;
    int closest_point = 0;

    for( int i = 0; i < points.size(); i++ )
    {
        float distance = Vector2Distance(points[i], target);
        if( distance < min_distance )
        {
            min_distance = distance;
            closest_point = i;
        }
    }

    return closest_point;
}

bool polygon_intersects_polygon(const Polygon& p1, const Polygon& p2, Vector2 &normal, float &depth )
{
    normal = {0, 0};
    depth = INFINITY;
    
    for( int i = 0; i < p1.points.size(); i++ )
    {
        const Vector2 vec_a = p1.points[i];
        const Vector2 vec_b = p1.points[(i + 1) % p1.points.size()];

        auto [x, y] = Vector2Subtract(vec_b, vec_a);
        const Vector2 axis = Vector2Normalize({-y, x});

        float p1_min, p1_max, p2_min, p2_max;
        project_polygon_to_axis(axis, p1, p1_min, p1_max);
        project_polygon_to_axis(axis, p2, p2_min, p2_max);

        if( p1_min >= p2_max - TOLERANCE || p2_min >= p1_max - TOLERANCE )
            return false;

        float tmp_depth = std::min(p2_max - p1_min, p1_max - p2_min);
        if( tmp_depth < depth )
        {
            depth = tmp_depth;
            normal = axis;
        }
    }
    
    for( int i = 0; i < p2.points.size(); i++ )
    {
        const Vector2 vec_a = p2.points[i];
        const Vector2 vec_b = p2.points[(i + 1) % p2.points.size()];

        auto [x, y] = Vector2Subtract(vec_b, vec_a);
        const Vector2 axis = Vector2Normalize({-y, x});

        float p1_min, p1_max, p2_min, p2_max;
        project_polygon_to_axis(axis, p1, p1_min, p1_max);
        project_polygon_to_axis(axis, p2, p2_min, p2_max);

        if( p1_min >= p2_max - TOLERANCE || p2_min >= p1_max - TOLERANCE )
            return false;

        float tmp_depth = std::min(p2_max - p1_min, p1_max - p2_min);
        if( tmp_depth < depth )
        {
            depth = tmp_depth;
            normal = axis;
        }
    }

    Vector2 direction = Vector2Subtract(p2.origin, p1.origin);

    if( Vector2DotProduct(direction, normal) > 0 )
        normal = Vector2Negate(normal);
    
    return true;
}

bool polygon_intersects_circle(const Polygon& p, const Circle& c, Vector2& normal, float& depth)
{
    normal = {0, 0};
    depth = INFINITY;
    float p_min, p_max, c_min, c_max;
    
    for( int i = 0; i < p.points.size(); i++ )
    {
        const Vector2 vec_a = p.points[i];
        const Vector2 vec_b = p.points[(i + 1) % p.points.size()];

        auto [x, y] = Vector2Subtract(vec_b, vec_a);
        const Vector2 axis = Vector2Normalize({-y, x});

        project_polygon_to_axis(axis, p, p_min, p_max);
        project_circle_to_axis(axis, c, c_min, c_max);

        if( p_min >= c_max - TOLERANCE || c_min >= p_max - TOLERANCE )
            return false;

        float tmp_depth = std::min(c_max - p_min, p_max - c_min);
        if( tmp_depth < depth )
        {
            depth = tmp_depth;
            normal = axis;
        }
    }

    Vector2 closest_point = p.points[closest_point_to_target(p.points, c.origin)];
    Vector2 direction = Vector2Subtract(c.origin, closest_point);
    direction = Vector2Normalize(direction);

    project_polygon_to_axis(direction, p, p_min, p_max);
    project_circle_to_axis(direction, c, c_min, c_max);

    if( p_min >= c_max - TOLERANCE || c_min >= p_max - TOLERANCE )
        return false;
    
    float tmp_depth = std::min(c_max - p_min, p_max - c_min);
    if( tmp_depth < depth )
    {
        depth = tmp_depth;
        normal =direction;
    }

    direction = Vector2Subtract(p.origin, c.origin);

    if( Vector2DotProduct(direction, normal) < 0 )
        normal = Vector2Negate(normal);
    
    return true;
}
