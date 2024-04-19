﻿#ifndef ENTITY_H
#define ENTITY_H
#include <raylib.h>

enum EntityState
{
    GROUNDED,
    IDLE,
    WALKING,
    RUNNING,
    ATTACKING,
    DYING
};

enum EntityTeam
{
    FRIENDLY,
    NEUTRAL,
    ENEMY
};

enum EntityTag
{
    WORLD,
    OBJECT,
    COLLECTIBLE,
    PLAYER,
    CHARACTER,
    NONE
};

class Entity
{
public:
    Entity(const Vector3 position, const char* model_path, const char* name);
    Entity(Entity&&) = delete; // Move constructor
    Entity(const Entity&) = delete; // Copy constructor
    Entity& operator=(Entity&&) = delete; // Move assignment operator
    Entity& operator=(const Entity&) = delete; // Copy assignment operator
    virtual ~Entity(); // Destructor, needed for virtual function overrides?
    
    virtual void update();
    virtual void update_fixed();
    void draw() const;

private:

    int id_;
    const char* name_;
    
    EntityTag tag_;
    EntityTeam team_;
    EntityState state_;
    
    bool visibility_;
    bool collision_;

    Model model_;

    Vector3 position_;
    Vector3 rotation_;
    Vector3 scale_;
};

#endif //ENTITY_H