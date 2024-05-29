#include "player.h"
#include <raymath.h>

Polygon* poly;
Polygon* poly2;
Body* c1;
Body* c3;
Body* c32;

Player::Player(const Vector2 position, const int width, const int height)
{
    this->position = position;
    this->width = width;
    this->height = height;

    Vector2 offset = {width / 2.0f, height / 2.0f};
    this->polygon = new Polygon();
    this->polygon->origin = position;
    this->polygon->points.push_back( Vector2Add(position, {-offset.x, -offset.y}) );
    this->polygon->points.push_back( Vector2Add(position, { offset.x, -offset.y}) ); 
    this->polygon->points.push_back( Vector2Add(position, { offset.x,  offset.y}) );
    this->polygon->points.push_back( Vector2Add(position, {-offset.x,  offset.y}) );
    
    c1 = new Body(polygon);
    c1->is_static = false;
    c1->parent = nullptr;
    c1->type = POLYGON;
    c1->polygon = polygon;
    c1->width = width * 10;
    c1->height = height * 10;
    c1->mass = 100.0f;
    c1->calculate_rotational_inertia();
    Collision::add_collider(c1);

    poly2 = new Polygon();
    poly2->origin = {200, 210};
    poly2->points.push_back({0, 0});
    poly2->points.push_back({0, 20});
    poly2->points.push_back({400, 420});
    poly2->points.push_back({400, 400});
    c3 = new Body(poly2);
    c3->is_static = true;
    c3->parent = nullptr;
    c3->type = POLYGON;
    c3->polygon = poly2;
    c3->width = 400;
    c3->rotation_static = true;
    c3->height = 20;
    c3->mass = 100.0f;
    c3->restitution = 0.5f;
    c3->calculate_rotational_inertia();
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
        c1->apply_force({0, -25000.0f});
    if( IsKeyDown(KEY_S ))
        c1->apply_force({0, 25000.0f});
    if( IsKeyDown(KEY_A ))
        c1->apply_force({-25000.0f, 0});
    if( IsKeyDown(KEY_D ))
        c1->apply_force({25000.0f, 0});
    if( IsKeyDown(KEY_RIGHT))
        c1->angular_velocity = 100;
    if( IsKeyDown(KEY_LEFT))
        c1->angular_velocity = -100;
    if( IsKeyDown(KEY_SPACE))
        c1->apply_force({0, -50});

    //Jump
    if( IsKeyDown(KEY_SPACE))
    {
        //c1->apply_force({0, -5000000.0f});
        //if(IsKeyReleased(KEY_SPACE))
            c1->apply_force({0, -5000000.0f});
    }

    if( IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Polygon* square = new Polygon();
        Body* square_c = new Body(square);
        square->origin = GetMousePosition();
        square->points.push_back({square->origin.x - 10, square->origin.y -10});
        square->points.push_back({square->origin.x + 10, square->origin.y -10});
        square->points.push_back({square->origin.x + 10, square->origin.y +10});
        square->points.push_back({square->origin.x - 10, square->origin.y +10});
        square_c->is_static = false;
        square_c->parent = nullptr;
        square_c->type = POLYGON;
        square_c->polygon = square;
        square_c->width = 20;
        square_c->height = 20;
        square_c->mass = 100.0f;
        square_c->restitution = 0.8f;
        square_c->calculate_rotational_inertia();
        Collision::add_collider(square_c);
        
    }

    Vector2 offset = Vector2Subtract(position, polygon->origin);
    c1->translate(offset);
}


void Player::draw()
{
    for( int i = 0; i < polygon->points.size(); i++ )
    {
        DrawLine(polygon->origin.x, polygon->origin.y, polygon->points[i].x, polygon->points[i].y, BLUE);
        DrawLine(polygon->points[i].x, polygon->points[i].y, polygon->points[(i + 1) % polygon->points.size()].x, polygon->points[(i + 1) % polygon->points.size()].y, SKYBLUE);
    }
    DrawLine(polygon->origin.x, polygon->origin.y, position.x, position.y, RED);
    DrawCircle(position.x, position.y, 5, SKYBLUE);

    const char* velocity = TextFormat("Velocity X: %f Y: %f", c1->velocity.x, c1->velocity.y);
    DrawText(velocity, 10, 10, 20, RED);

}
