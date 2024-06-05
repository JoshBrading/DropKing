#include "player.h"
#include <raymath.h>

void check_grounded(cpBody* body, cpArbiter* arb, void* player_entity)
{
    auto player = (Game::Entities::Player*)player_entity;
    if( !player ) return;
        
    cpShape *a, *b;
    cpArbiterGetShapes(arb, &a, &b);
    if( player->get_player_object()->shape != a && player->get_player_object()->shape != b) return;

    Physics::ObjectDetails* details = (Physics::ObjectDetails*)cpShapeGetUserData(b);
    if( !details) return;
        
    cpVect n = cpArbiterGetNormal(arb);
    cpArbiterSetRestitution(arb, 0.0f);
        
    if( details->tag == Physics::ObjectDetails::GROUND)
    {
        player->is_grounded = true;
        // object->ground = b;
    }
        
}

Game::Entities::Player::Player(Vector2 position): Entity(position, 0)
{
    this->position = position;
    this->spawn_point = position;
    this->player_object = Physics::create_square(position, {48, 48});
    this->tag = PLAYER;
    this->player_background = LoadTexture("assets\\player\\base.png");
    this->player_face = LoadTexture("assets\\player\\face.png");
    this->texture_offset = Vector2{player_background.width / 2.0f, player_background.height / 2.0f};
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
    //jump_scale = jump_scale + 1;
    
    DrawText(TextFormat("Jump: %f", jump_scale), 10, 30, 20, WHITE);
    cpVect curr_vel = cpBodyGetVelocity(player_object->body);
    cpBodyEachArbiter(player_object->body, check_grounded, this);
    if( is_grounded )
    {
            
        if( IsKeyDown(KEY_SPACE))
        {
            if( jump_scale < max_jump_scale)
                jump_scale += jump_scale_increase * GetFrameTime();
        }
        else if( IsKeyReleased(KEY_SPACE))
        {
            cpVect jump_impulse = cpv(0, -(max_jump_height + max_jump_height * jump_scale));
            cpBodyApplyImpulseAtWorldPoint(player_object->body, jump_impulse, cpBodyGetPosition(player_object->body));
            cpBodySetAngularVelocity(player_object->body, 100 * PI / 180);
            is_grounded = false;
            jump_scale = 0.0f;
        }
    }
    
    cpVect pos = cpBodyGetPosition(player_object->body);
    position = Vector2{(float)pos.x, (float)pos.y};

    rotation = cpBodyGetAngle(player_object->body);
}

void Game::Entities::Player::draw()
{
    Entity::draw();
    
    Rectangle rect = {position.x, position.y, player_object->size.x, player_object->size.y};
    DrawTexturePro(player_background, {0, 0, 48, 48}, rect, texture_offset, rotation * RAD2DEG, WHITE);
    DrawTexturePro(player_face, {0, 0, 48, 48}, rect, texture_offset, rotation * RAD2DEG, WHITE);
}

void Game::Entities::Player::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
{
    Entity::on_collision(arb, space, entity);
    if( entity )
    {
        
    }
}
