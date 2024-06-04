#include <format>
#include <string>
#include "entity.h"
#include <raymath.h>
#include "entity_manager.h"


Entity::Entity(Vector2 position, float deg_rotation, std::string sprite_path)
{
}

Entity::Entity(Vector2 position, float deg_rotation)
{
    this->id = -1;
    this->position = position;
    this->target_position = position;
    this->rotation = deg_rotation;
    this->scale = {1.0f, 1.0f};
    this->tag = EntityTag::NONE;
    this->team = EntityTeam::NEUTRAL;
    this->name = "Entity";
    
}

Entity::~Entity()
= default;

void Entity::update()
{
}

void Entity::update_fixed()
{
    //position_.x += 10;
}

void Entity::draw()
{

    //DrawModel(model, position, 1.0f, WHITE);
}

void Entity::draw_debug(const Camera2D& camera)
{
    //const auto [x, y] = GetWorldToScreen(position, camera);
    //int length = MeasureText(name, 10);
    //const std::string position = "( X:" + std::format("{:.2f}", this->position.x) + " Y: " + std::format("{:.2f}", this->position.y) + " Z: " + std::format("{:.2f}", this->position.z) + " )";
    //DrawText(name, static_cast<int>(x) - (length / 2), static_cast<int>(y) + 30, 10, YELLOW);
    //length = MeasureText(position.c_str(), 10);
    //DrawText(position.c_str(), static_cast<int>(x) - (length / 2), static_cast<int>(y) + 45, 10, WHITE);
}

void Entity::on_collision(cpArbiter* arb, cpSpace* space, Entity* entity)
{
}

int Entity::get_id() const
{
    return id;
}

EntityTag Entity::get_tag() const
{
    return tag;
}

EntityTeam Entity::get_team() const
{
    return team;
}

Vector2 Entity::get_target_position() const
{
    return target_position;
}

void Entity::set_target_position(const Vector2 target_position)
{
    this->target_position = target_position;
}

void Entity::set_id(const int id)
{
    this->id = id;
}

void Entity::set_tag(const EntityTag tag)
{
    this->tag = tag;
}

void Entity::set_team(const EntityTeam team)
{
    this->team = team;
}

Vector2 Entity::get_position() const
{
    return position;
}

const char* Entity::get_name() const
{
    return name;
}
