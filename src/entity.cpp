#include <format>
#include <string>
#include "entity.h"
#include <raymath.h>
#include "entity_manager.h"

Entity::Entity(const Vector3 position, const char* model_path, const char* name)
{
    this->id = 0;
    this->name = name;
    this->tag = EntityTag::NONE;
    this->team = EntityTeam::NEUTRAL;
    this->state = EntityState::IDLE;
    this->model = LoadModel(model_path);
    
    this->position = position;
    this->target_position = position;
    this->rotation = Vector3Zero();
    this->scale = Vector3One();
    
    this->visibility = true;
    this->collision = true;

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

    DrawModel(model, position, 1.0f, BLUE);
}

void Entity::draw_debug(const Camera& camera)
{
    const auto [x, y] = GetWorldToScreen(position, camera);
    int length = MeasureText(name, 10);
    const std::string position = "( X:" + std::format("{:.2f}", this->position.x) + " Y: " + std::format("{:.2f}", this->position.y) + " Z: " + std::format("{:.2f}", this->position.z) + " )";
    DrawText(name, static_cast<int>(x) - (length / 2), static_cast<int>(y) + 30, 10, YELLOW);
    length = MeasureText(position.c_str(), 10);
    DrawText(position.c_str(), static_cast<int>(x) - (length / 2), static_cast<int>(y) + 45, 10, WHITE);
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

Vector3 Entity::get_target_position() const
{
    return target_position;
}

void Entity::set_target_position(const Vector3 target_position )
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

Vector3 Entity::get_position() const
{
    return position;
}

const char* Entity::get_name() const
{
    return name;
}
