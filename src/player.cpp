#include "player.h"
#include <raymath.h>

Polygon* poly;
Body* c1;
Body* c3;

Player::Player(const Vector2 position, const int width, const int height)
{
    this->position = position;
    this->width = width;
    this->height = height;

    Vector2 offset = {width / 2.0f, height / 2.0f};
    this->polygon = new Polygon();
    this->polygon->origin = position;
    this->polygon->points.push_back( Vector2Add(position, {-offset.x, -offset.y}) );
    this->polygon->points.push_back( Vector2Add(position, { 15,  -80}) );
    this->polygon->points.push_back( Vector2Add(position, { offset.x, -offset.y}) ); 
    this->polygon->points.push_back( Vector2Add(position, { 70,  20}) );
    this->polygon->points.push_back( Vector2Add(position, { offset.x,  offset.y}) );
    this->polygon->points.push_back( Vector2Add(position, { 20,  70}) );
    this->polygon->points.push_back( Vector2Add(position, {-offset.x,  offset.y}) );
    this->polygon->points.push_back( Vector2Add(position, { -70,  20}) );

    c1 = new Body(polygon);
    c1->is_static = false;
    c1->parent = nullptr;
    c1->type = POLYGON;
    c1->polygon = polygon;
    Collision::add_collider(c1);
    
    poly = new Polygon();
    poly->origin = {400, 400};
    poly->points.push_back({350, 350});
    poly->points.push_back({450, 350});
    poly->points.push_back({450, 450});
    poly->points.push_back({350, 450});

    c3 = new Body(poly);
    c3->is_static = true;
    c3->parent = nullptr;
    c3->type = POLYGON;
    c3->polygon = poly;
    Collision::add_collider(c3);
}


Vector2 LINE_START = {200, 100};
Vector2 LINE_END = {400, 500};
void Player::update()
{
    position = polygon->origin;
    Vector2 normal = {0, 0};
    float depth = 0;

    if( IsKeyDown(KEY_W ))
        c1->apply_force({0, -5.0f});
    if( IsKeyDown(KEY_S ))
        c1->apply_force({0, 5.0f});
    if( IsKeyDown(KEY_A ))
        c1->apply_force({-5.0f, 0});
    if( IsKeyDown(KEY_D ))
        c1->apply_force({5.0f, 0});
    if( IsKeyDown(KEY_RIGHT))
        c1->rotate(2);
    if( IsKeyDown(KEY_LEFT))
        c1->rotate(-2);

    Vector2 offset = Vector2Subtract(position, polygon->origin);
    c1->translate(offset);
    c3->rotate(-2);
    

}


void Player::draw()
{
   // for( int i = 0; i < polygon->points.size(); i++ )
   // {
   //     DrawLine(polygon->origin.x, polygon->origin.y, polygon->points[i].x, polygon->points[i].y, BLUE);
   //     DrawLine(polygon->points[i].x, polygon->points[i].y, polygon->points[(i + 1) % polygon->points.size()].x, polygon->points[(i + 1) % polygon->points.size()].y, SKYBLUE);
   // }
   // DrawLine(polygon->origin.x, polygon->origin.y, position.x, position.y, RED);
   // DrawCircle(position.x, position.y, 5, SKYBLUE);

    

}
