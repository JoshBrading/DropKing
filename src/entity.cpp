#include "entity.h"
#include <raymath.h>

#include "entity_manager.h"

Entity::Entity(const Vector3 position, const char* model_path, const char* name)
{
    id_ = 0;
    name_ = name;
    tag_ = EntityTag::NONE;
    team_ = EntityTeam::NEUTRAL;
    state_ = EntityState::IDLE;
    model_ = LoadModel(model_path);
    
    position_ = position;
    rotation_ = Vector3Zero();
    scale_ = Vector3One();
    
    visibility_ = true;
    collision_ = true;

}

Entity::~Entity()
{
}

void Entity::update()
{
}

void Entity::update_fixed()
{
    //position_.x += 10;
}

void Entity::draw()
{

    DrawModel(model_, position_, 1.0f, BLUE);
}

void Entity::draw_debug(const Camera& camera)
{
    const Vector2 screen_position = GetWorldToScreen(position_, camera);
    const int length = MeasureText(name_, 10);
    DrawText(name_, static_cast<int>(screen_position.x) - length, static_cast<int>(screen_position.y), 20, RED);
}

int Entity::get_id() const
{
    return id_;
}

EntityTag Entity::get_tag() const
{
    return tag_;
}

EntityTeam Entity::get_team() const
{
    return team_;
}

void Entity::set_id(const int id)
{
    id_ = id;
}

void Entity::set_tag(const EntityTag tag)
{
    tag_ = tag;
}

void Entity::set_team(const EntityTeam team)
{
    team_ = team;
}

const char* Entity::get_name() const
{
    return name_;
}
