#include "player.h"
#include <raymath.h>


Game::Entities::Player::Player(Vector2 position): Entity(position, 0)
{
    this->position = position;
    this->spawn_point = position;
    this->player_object = Physics::create_square(position, {48, 48});
    this->tag = PLAYER;
    Physics::ObjectDetails* details = new Physics::ObjectDetails();
    details->data = this;
    details->tag = Physics::ObjectDetails::PLAYER;
    cpShapeSetUserData(player_object->shape, details);
    cpShapeSetFriction(player_object->shape, 0.0f);
}

void Game::Entities::Player::reset_player()
{
    cpBodySetPosition(player_object->body, cpv(spawn_point.x, spawn_point.y));
    cpBodySetVelocity(player_object->body, cpvzero);
    cpBodySetAngularVelocity(player_object->body, 0);
    cpBodySetAngle(player_object->body, 0);
}

void Game::Entities::Player::set_spawn_point(Vector2 spawn_point)
{
    this->spawn_point = spawn_point;
}

Physics::Object* Game::Entities::Player::get_player_object()
{
    return player_object;
}

void Game::Entities::Player::update()
{
    Entity::update();
    player_object->update();
    cpVect pos = cpBodyGetPosition(player_object->body);
    position = Vector2{(float)pos.x, (float)pos.y};
}

void Game::Entities::Player::draw()
{
    Entity::draw();
    cpVect pos = cpBodyGetPosition(player_object->body);
    float a = cpBodyGetAngle(player_object->body);
    Rectangle rect = {(float)pos.x, (float)pos.y, player_object->size.x, player_object->size.y};
    DrawRectanglePro(rect, {player_object->size.x / 2, player_object->size.y / 2},
                     cpBodyGetAngle(player_object->body) * RAD2DEG,RED);
}

void Game::Entities::Player::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
{
    Entity::on_collision(arb, space, entity);
    
}
