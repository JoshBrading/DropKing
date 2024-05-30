#include "player.h"
#include <raymath.h>

#include "PhysicsWorld.h"

Polygon* poly;
Polygon* poly2;
Body* c1;
Body* c3;
Body* c32;

std::string s;

Physics::Body test;
Physics::Body test2;
Physics::Body test3;

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
    /*
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
    Collision::add_collider(c3);*/
    
    Physics::BodyFactory::create_box_body(width, height, 10.0f, false, 0.8f, test, s);
    Physics::Instance::WORLD.add_body(&test);
    test.set_position(position);

    Physics::BodyFactory::create_box_body(width * 4, height, 10.0f, true, 0.8f, test2, s);
    Physics::Instance::WORLD.add_body(&test2);
    test2.set_position({300, 100});
    test2.set_rotation(120);

    Physics::BodyFactory::create_box_body(width * 6, height, 10.0f, true, 0.8f, test3, s);
    Physics::Instance::WORLD.add_body(&test3);
    test3.set_position({650, 350});
    test3.set_rotation(-45);
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
        
        //Physics::Body* test = new Physics::Body(1.0f, 1.0f, 1.0f, 0.5f, 1.0f, false, 10, 10, 10, this->polygon->points, Physics::BOX);

    }

    //Vector2 offset = Vector2Subtract(position, polygon->origin);
    //c1->translate(offset);
    Physics::Instance::WORLD.update(GetFrameTime(), 20);
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

    const char* testpost = TextFormat("Test Position X: %f Y: %f", test.get_position().x, test.get_position().y);
    DrawText(testpost, 10, 10, 20, RED);

    DrawText(s.c_str(), 10, 30, 20, RED);

    

}
