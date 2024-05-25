#include "player.h"
#include <raymath.h>

Polygon MOCK;
Circle circle;
Player::Player(const Vector2 position, const int width, const int height)
{
    this->position = position;
    this->width = width;
    this->height = height;

    Vector2 offset = {width / 2.0f, height / 2.0f};
    this->polygon.origin = position;
    this->polygon.points.push_back( Vector2Add(position, {-offset.x, -offset.y}) );
    this->polygon.points.push_back( Vector2Add(position, { 15,  -80}) );
    this->polygon.points.push_back( Vector2Add(position, { offset.x, -offset.y}) ); 
    this->polygon.points.push_back( Vector2Add(position, { 70,  20}) );
    this->polygon.points.push_back( Vector2Add(position, { offset.x,  offset.y}) );
    this->polygon.points.push_back( Vector2Add(position, { 20,  70}) );
    this->polygon.points.push_back( Vector2Add(position, {-offset.x,  offset.y}) );
    this->polygon.points.push_back( Vector2Add(position, { -70,  20}) );

    
    MOCK.origin = {250, 450};
    MOCK.points.push_back({200, 400});
    MOCK.points.push_back({200, 500});
    MOCK.points.push_back({300, 500});
    MOCK.points.push_back({300, 400});

    circle.origin = {400, 400};
    circle.radius = 25;
}

Vector2 rotate_point_about_target(const Vector2 origin, Vector2 point, const float angle) {
    const float radians = angle * PI / 180.0f;

    const float s = sinf(radians);
    const float c = cosf(radians);

    point.x -= origin.x;
    point.y -= origin.y;

    const Vector2 rotation = {point.x * c - point.y * s, point.x * s + point.y * c};
    const Vector2 rotated_point = Vector2Add(rotation, origin);

    return rotated_point;
}


Vector2 LINE_START = {200, 100};
Vector2 LINE_END = {400, 500};
void Player::update()
{
    Vector2 normal = {0, 0};
    float depth = 0;

    if( IsKeyDown(KEY_W ))
        position = Vector2Add(position, {0, -4});
    if( IsKeyDown(KEY_S ))
        position = Vector2Add(position, {0, 4});
    if( IsKeyDown(KEY_A ))
        position = Vector2Add(position, {-4, 0});
    if( IsKeyDown(KEY_D ))
        position = Vector2Add(position, {4, 0});

    if( IsKeyDown( KEY_RIGHT ) )
    {
        for( auto& point: polygon.points )
        {
            point = rotate_point_about_target(position, point, 2);
        }
    }
    if( IsKeyDown( KEY_LEFT ) )
    {
        for( auto& point: polygon.points )
        {
            point = rotate_point_about_target(position, point, -2);
        }
    }

    Vector2 offset = Vector2Subtract(position, polygon.origin);
    polygon.origin = position;
    for( auto& point: polygon.points )
    {
        point = Vector2Add(point, offset);
    }  
    if( polygon_intersects_polygon(polygon, MOCK, normal, depth) )
    {
        Vector2 displacement = Vector2Scale(normal, depth / 2);
        position = Vector2Add(position, displacement);
        polygon.origin = position;
        for( auto& point: polygon.points )
        {
            point = Vector2Add(point, displacement);
        }

        MOCK.origin = Vector2Subtract(MOCK.origin, displacement);
        for( auto& point: MOCK.points )
        {
            point = Vector2Subtract(point, displacement);
        }
    }

    if( polygon_intersects_circle(polygon, circle, normal, depth) )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        position = Vector2Add(position, displacement);
        polygon.origin = position;
        for( auto& point: polygon.points )
        {
            point = Vector2Add(point, displacement);
        }   
    }

    if( polygon_intersects_circle(MOCK, circle, normal, depth) )
    {
        Vector2 displacement = Vector2Scale(normal, depth);
        MOCK.origin = Vector2Add(MOCK.origin, displacement);
        for( auto& point: MOCK.points )
        {
            point = Vector2Add(point, displacement);
        }   
    }
}


void Player::draw()
{
    for( int i = 0; i < polygon.points.size(); i++ )
    {
        DrawLine(polygon.origin.x, polygon.origin.y, polygon.points[i].x, polygon.points[i].y, BLUE);
        DrawLine(polygon.points[i].x, polygon.points[i].y, polygon.points[(i + 1) % polygon.points.size()].x, polygon.points[(i + 1) % polygon.points.size()].y, SKYBLUE);
    }
    DrawCircle(position.x, position.y, 5, SKYBLUE);

    DrawCircle(circle.origin.x, circle.origin.y, circle.radius, RED);

    for( int i = 0; i < MOCK.points.size(); i++ )
    {
        DrawLine(MOCK.origin.x, MOCK.origin.y, MOCK.points[i].x, MOCK.points[i].y, YELLOW);
        DrawLine(MOCK.points[i].x, MOCK.points[i].y, MOCK.points[(i + 1) % MOCK.points.size()].x, MOCK.points[(i + 1) % MOCK.points.size()].y, GOLD);
    }
    DrawCircle(MOCK.origin.x, MOCK.origin.y, 5, GOLD);
}
