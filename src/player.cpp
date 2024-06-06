#include "player.h"
#include <raymath.h>
#include <cmath>
#include <filesystem>
#include <queue>

#include "game.h"

struct player_snapshot
{
    cpVect position;
    cpFloat angular_velocity;
    cpVect velocity;
    cpFloat angle;
    int score;
};

std::queue<player_snapshot> HISTORY_QUEUE;
float LAST_QUEUE_TIME = 0.0f;

void check_grounded(cpBody* body, cpArbiter* arb, void* player_entity)
{
    const auto player = static_cast<Game::Entities::Player*>(player_entity);
    if( !player ) return;
        
    cpShape *a, *b;
    cpArbiterGetShapes(arb, &a, &b);
    if( player->get_player_object()->shape != a && player->get_player_object()->shape != b) return;

    const Physics::ObjectDetails* details = static_cast<Physics::ObjectDetails*>(cpShapeGetUserData(b));
    if( !details) return;
        
    cpArbiterSetRestitution(arb, 0.0f);

        
    if( details->tag == Physics::ObjectDetails::GROUND)
    {
        player->is_grounded = true;
        cpVect normal = cpSegmentShapeGetNormal(b);
        if( normal.y > 0.0f )
            normal = cpvneg(normal);
        player->set_ground_normal({static_cast<float>(normal.x), static_cast<float>(normal.y)});
    }
        
}

void Game::Entities::Player::take_snapshot() const
{
    player_snapshot snapshot;
    snapshot.position = cpBodyGetPosition(player_object->body);
    snapshot.angular_velocity = cpBodyGetAngularVelocity(player_object->body);
    snapshot.velocity = cpBodyGetVelocity(player_object->body);
    snapshot.angle = cpBodyGetAngle(player_object->body);
    snapshot.score = score;
    HISTORY_QUEUE.push(snapshot);
}

void Game::Entities::Player::calculate_score()
{
    cpVect velocity = cpBodyGetVelocity(player_object->body);
    if( velocity.y > 0 )
        score += (velocity.y + 20) * GetFrameTime();
    else
        score += 10 * GetFrameTime();
}

Game::Entities::Player::Player(Vector2 position): Entity(position, 0)
{
    this->position = position;
    this->spawn_point = position;
    this->player_object = Physics::create_square(position, {48, 48});
    this->tag = PLAYER;

    const char* faces[] = {"default", "tough", "angry"};
    const char* bases[] = {"default", "purple", "blue"};
    
    this->player_face = LoadTexture(TextFormat("assets\\player\\%s_face.png", faces[rand() % 3]));
    this->player_background = LoadTexture(TextFormat("assets\\player\\%s_base.png", bases[rand() % 3]));
    
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
    hearts = 3;
    score = 0;
    level_complete = false;
}

void Game::Entities::Player::set_spawn_point(const Vector2 spawn_point)
{
    this->spawn_point = spawn_point;
}

void Game::Entities::Player::set_ground_normal(const Vector2 normal)
{
    this->ground_normal = {normal.x, normal.y};
}

int Game::Entities::Player::get_hearts() const
{
    return hearts;
}

float Game::Entities::Player::get_score() const
{
    return score;
}

void Game::Entities::Player::set_score( float new_score )
{
    score = new_score;
}

Physics::Object* Game::Entities::Player::get_player_object() const
{
    return player_object;
}

void Game::Entities::Player::update()
{
    Entity::update();
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
            const cpVect curr_vel = cpBodyGetVelocity(player_object->body);
            auto [angle_x, angle_y] = cpvforangle(std::atan2(ground_normal.y, ground_normal.x));
            auto [jump_impulse_x, jump_impulse_y] = Vector2Scale({static_cast<float>(angle_x) / 2.0f, static_cast<float>(angle_y)}, (max_jump_height + max_jump_height * jump_scale));
            cpBodyApplyImpulseAtWorldPoint(player_object->body, {jump_impulse_x, jump_impulse_y}, cpBodyGetPosition(player_object->body));
            if( curr_vel.x > 0)
                cpBodySetAngularVelocity(player_object->body, 100 * DEG2RAD);
            else if( curr_vel.x < 0)
                cpBodySetAngularVelocity(player_object->body, -100 * DEG2RAD);
            is_grounded = false;
            jump_scale = 0.0f;
        }
    }

    cpVect vel = cpBodyGetVelocity(player_object->body);
    if( IsKeyDown(KEY_A) && vel.x > 0)
    {
        vel.x -= 500 * GetFrameTime();
        cpBodySetVelocity(player_object->body, vel);
    }
    else if( IsKeyDown(KEY_D) && vel.x < 0)
    {
        vel.x += 500 * GetFrameTime();
        cpBodySetVelocity(player_object->body, vel);
    }
    
    cpVect pos = cpBodyGetPosition(player_object->body);
    position = Vector2{static_cast<float>(pos.x), static_cast<float>(pos.y)};

    rotation = cpBodyGetAngle(player_object->body);
    
    if( GetTime() - LAST_QUEUE_TIME > 1.0f / 60.0f)
    {
        take_snapshot();
        LAST_QUEUE_TIME = GetTime();
    }
    if( HISTORY_QUEUE.size() > 90)
        HISTORY_QUEUE.pop();

    if( !level_complete && !Game::PAUSE)
        calculate_score();
}

void Game::Entities::Player::draw()
{
    Entity::draw();
    
    Rectangle rect = {position.x, position.y, player_object->size.x, player_object->size.y};
    DrawTexturePro(player_background, {0, 0, 48, 48}, rect, texture_offset, rotation * RAD2DEG, WHITE);
    DrawTexturePro(player_face, {0, 0, 48, 48}, rect, texture_offset, rotation * RAD2DEG, WHITE);

    //DrawLineEx({position.x, position.y}, {position.x + ground_normal.x * 32, position.y + ground_normal.y * 32}, 4, RED);
    //DrawLineEx( {position.x, position.y}, {position.x + ground_normal.y * 32, position.y - ground_normal.x * 32}, 4, BLUE);
    //DrawLineEx({position.x, position.y}, {position.x + ground_normal.x * 100 * jump_scale/ max_jump_scale, position.y + ground_normal.y * 100* jump_scale/ max_jump_scale}, 8, WHITE);
}

void Game::Entities::Player::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
{
    Entity::on_collision(arb, space, entity);
    if( entity )
    {
        if( entity->get_tag() == SPIKE_PIT)
        {
            if( hearts > 0)
            {
                cpBodySetPosition(player_object->body, HISTORY_QUEUE.front().position);
                cpBodySetVelocity(player_object->body, HISTORY_QUEUE.front().velocity);
                cpBodySetAngularVelocity(player_object->body, HISTORY_QUEUE.front().angular_velocity);
                cpBodySetAngle(player_object->body, HISTORY_QUEUE.front().angle);
                score = HISTORY_QUEUE.front().score;
                while( !HISTORY_QUEUE.empty() )
                    HISTORY_QUEUE.pop();
                hearts--;
            }
        }
        if( entity->get_tag() == FINISH_BOX )
            level_complete = true;
    }
}

