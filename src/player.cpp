#include "player.h"
#include <raymath.h>

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
}


void Player::update()
{
}


void Player::draw()
{
    
}
