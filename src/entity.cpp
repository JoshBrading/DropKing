#include "entity.h"
#include <raymath.h>

Entity::Entity(const Vector3 position, const char* model_path, const char* name)
{
    id_ = 0;
    tag_ = '0';
    name_ = name;
    
    model_ = LoadModel(model_path);
    
    position_ = position;
    rotation_ = Vector3Zero();
    scale_ = Vector3One();
    
    visibility_ = true;
    collision_ = true;

}

Entity::~Entity() = default;

void Entity::update()
{
    DrawText(name_, static_cast<int>(position_.x), static_cast<int>(position_.y), 10, RED);
}

void Entity::update_fixed()
{
    position_.x += 10;
}

void Entity::draw() const
{
}
