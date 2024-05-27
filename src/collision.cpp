﻿#include "collision.h"
#include <raymath.h>

#include "transform.h"

float TOLERANCE = 0.01f;

Collision& Collision::instance()
{
    static auto instance = Collision();
    return instance;
}

void Body::rotate(float degrees) const
{
    if( type != POLYGON )
        return;
    for( auto& point: polygon->points )
    {
        point = transform::rotate_point_about_target(polygon->origin, point, degrees);
    }
}

void Body::translate( Vector2 translation ) const
{
    if( type == CIRCLE )
    {
        circle->origin = Vector2Add(circle->origin, translation);
    }
    if( type == POLYGON )
    {
        polygon->origin = Vector2Add(polygon->origin, translation);
        for( auto& point: polygon->points )
        {
            point = Vector2Add(point, translation);
        }
    }
}

void Body::apply_force( Vector2 force )
{
    this->force = force;
}


void project_polygon_to_axis(const Vector2 axis, const Polygon* p, float &min, float &max)
{
    min = INFINITY;
    max = -INFINITY;

    for( const auto& point: p->points )
    {
        float projection = Vector2DotProduct(point, axis);

        if( projection < min )
            min = projection;
        if( projection > max )
            max = projection;
    }
}

void project_circle_to_axis(const Vector2 axis, const Circle* c, float &min, float &max)
{
    float projection = Vector2DotProduct(c->origin, axis);
    min = projection - c->radius;
    max = projection + c->radius;

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

bool polygon_intersects_polygon(const Polygon* p1, const Polygon* p2, Vector2 &normal, float &depth)
{
    normal = {0, 0};
    depth = INFINITY;
    
    for( int i = 0; i < p1->points.size(); i++ )
    {
        const Vector2 vec_a = p1->points[i];
        const Vector2 vec_b = p1->points[(i + 1) % p1->points.size()];

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
    
    for( int i = 0; i < p2->points.size(); i++ )
    {
        const Vector2 vec_a = p2->points[i];
        const Vector2 vec_b = p2->points[(i + 1) % p2->points.size()];

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

    Vector2 direction = Vector2Subtract(p2->origin, p1->origin);

    if( Vector2DotProduct(direction, normal) > 0 )
        normal = Vector2Negate(normal);
    
    return true;
}

bool polygon_intersects_circle(const Polygon* p, const Circle* c, Vector2& normal, float& depth)
{
    normal = {0, 0};
    depth = INFINITY;
    float p_min, p_max, c_min, c_max;
    
    for( int i = 0; i < p->points.size(); i++ )
    {
        const Vector2 vec_a = p->points[i];
        const Vector2 vec_b = p->points[(i + 1) % p->points.size()];

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

    Vector2 closest_point = p->points[closest_point_to_target(p->points, c->origin)];
    Vector2 direction = Vector2Subtract(c->origin, closest_point);
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

    direction = Vector2Subtract(p->origin, c->origin);

    if( Vector2DotProduct(direction, normal) < 0 )
        normal = Vector2Negate(normal);
    
    return true;
}

bool check_collision(Body* a, Body* b, Vector2& normal, float& depth)
{
    if( a->is_static && b->is_static )
        return false;

    if( a->type == POLYGON ) // This is gross, clean up if possible
    {
        if( b->type == POLYGON )
            return polygon_intersects_polygon(a->polygon, b->polygon, normal, depth);
        if( b->type == CIRCLE )
            return polygon_intersects_circle(a->polygon, b->circle, normal, depth);
    }
    if( a->type == CIRCLE )
    {
        if( b->type == POLYGON )
            return polygon_intersects_circle(b->polygon, a->circle, normal, depth);
    }
}

void resolve_polygon_polygon(Polygon* a, Polygon* b, bool a_is_static, bool b_is_static, Vector2 normal, float depth)
{
    if( a_is_static && b_is_static )
        return;
    
    if( a_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        b->origin = Vector2Add(b->origin, displacement);
        for( auto& point: b->points )
        {
            point = Vector2Add(point, displacement);
        }
    }

    if( b_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        a->origin = Vector2Add(a->origin, displacement);
        for( auto& point: a->points )
        {
            point = Vector2Add(point, displacement);
        }
    }

    if( !a_is_static && !b_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth / 2);
        a->origin = Vector2Add(a->origin, displacement);
        for( auto& point: a->points )
        {
            point = Vector2Add(point, displacement);
        }

        b->origin = Vector2Subtract(b->origin, displacement);
        for( auto& point: b->points )
        {
            point = Vector2Subtract(point, displacement);
        }
    }
    
}

void resolve_polygon_circle(Polygon* a, Circle* b, bool a_is_static, bool b_is_static, Vector2 normal, float depth)
{
    if( a_is_static && b_is_static )
        return;
    
    if( a_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        b->origin = Vector2Add(b->origin, displacement);
    }

    if( b_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        a->origin = Vector2Add(a->origin, displacement);
        for( auto& point: a->points )
        {
            point = Vector2Add(point, displacement);
        }
    }

    if( !a_is_static && !b_is_static )
    {
        Vector2 displacement = Vector2Scale(normal, depth / 2);
        a->origin = Vector2Add(a->origin, displacement);
        for( auto& point: a->points )
        {
            point = Vector2Add(point, displacement);
        }

        b->origin = Vector2Subtract(b->origin, displacement);
    }
}

void resolve_circle_polygon(Circle* circle, Polygon* polygon, bool circle_is_static, bool polygon_is_static, Vector2 normal, float depth)
{
    resolve_polygon_circle(polygon, circle, polygon_is_static, circle_is_static, normal, depth);

}

void resolve(Body* a, Body* b, Vector2 normal, float depth)
{
    if( a->is_static && b->is_static )
        return;
    
    if( a->type == POLYGON ) // This is gross, clean up if possible
    {
        if( b->type == POLYGON )
            resolve_polygon_polygon(a->polygon, b->polygon, a->is_static, b->is_static, normal, depth);
        if( b->type == CIRCLE )
            resolve_polygon_circle(a->polygon, b->circle, a->is_static, b->is_static, normal, depth);
    }
    if( a->type == CIRCLE )
    {
        if( b->type == POLYGON )
            resolve_circle_polygon(a->circle, b->polygon, a->is_static, b->is_static, normal, depth);
    }
    
    float e = std::min(a->restitution, b->restitution);
    float relative_velocity = Vector2DotProduct(Vector2Subtract(b->velocity, a->velocity), normal);
    float mass_sum = (a->is_static ? 0.0f : 1.0f / a->mass) + (b->is_static ? 0.0f : 1.0f / b->mass);

    if (mass_sum == 0.0f) return;
    
    float j = -(1 + e) * relative_velocity / mass_sum;
    if( !a->is_static )
        a->velocity = Vector2Subtract(a->velocity, Vector2Scale(normal, j / a->mass));
    if( !b->is_static )
        b->velocity = Vector2Add(b->velocity, Vector2Scale(normal, j / b->mass));
}

void Collision::i_add_collider(Body* collider)
{
    colliders.push_back(collider);
}

void Collision::i_update()
{
    for( int i = 0; i < colliders.size() - 1; i++ )
    {
        Body* a = colliders[i];
        Vector2 acceleration = Vector2Scale(a->force, 1.0f / a->mass);
        a->velocity = Vector2Add(a->velocity, acceleration);
        a->force = {0, 0};
        a->translate(Vector2Scale(a->velocity, GetFrameTime()));
        for( int j = 0; j < colliders.size(); j++ )
        {
            if( i == j )
                continue;
            Vector2 normal;
            float depth;
            if( check_collision(colliders[i], colliders[j], normal, depth) )
            {
                resolve(colliders[i], colliders[j], normal, depth);
            }
        }
    }
}

void Collision::i_draw_debug() const
{
    for( const auto& collider: colliders )
    {
        if( collider->type == POLYGON )
        {
            for( int i = 0; i < collider->polygon->points.size(); i++ )
            {
                if( !collider->is_static )
                {
                    DrawCircle(collider->polygon->origin.x, collider->polygon->origin.y, 2, WHITE);
                    DrawLineEx(collider->polygon->points[i], collider->polygon->points[(i + 1) % collider->polygon->points.size()], 2, RED);
                }
                else
                {
                    DrawCircle(collider->polygon->origin.x, collider->polygon->origin.y, 2, WHITE);
                    DrawLineEx(collider->polygon->points[i], collider->polygon->points[(i + 1) % collider->polygon->points.size()], 2, SKYBLUE);
                }
            }
        }
        if( collider->type == CIRCLE )
        {
            DrawCircle(collider->circle->origin.x, collider->circle->origin.y, collider->circle->radius, RED);
            DrawLine( collider->circle->origin.x, collider->circle->origin.y, collider->circle->origin.x + collider->circle->radius, collider->circle->origin.y, RED);
        }
    }
}
