#pragma once

#include <raylib.h>
#include <string>
#include "physics.h"

enum EntityType
{
    TILE,
    MORTAR,
    SOLDIER,
};

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
    TIMED_PLATFORM,
    FALSE_PLATFORM,
    SPIKE_PIT,
    FINISH_BOX,
    NONE
};

class Entity
{
    friend class EntityManager;
public:
    Entity(Vector2 position, float deg_rotation, std::string sprite_path);
    Entity(Vector2 position, float deg_rotation);
    Entity(Entity&&) = delete;
    Entity(const Entity&) = delete;
    Entity& operator=(Entity&&) = delete;
    Entity& operator=(const Entity&) = delete;
    virtual ~Entity();
    
    virtual void update();
    virtual void update_fixed();
    virtual void draw();
    virtual void draw_debug(const Camera2D& camera);
    virtual void on_collision(cpArbiter* arb, cpSpace* space, Entity* entity);

    int get_id() const;
    const char* get_name() const;
    
    EntityTag get_tag() const;
    void set_tag(EntityTag tag);
    
    EntityTeam get_team() const;
    void set_team(EntityTeam team);

    Vector2 get_position() const;

    Vector2 get_target_position() const;
    void set_target_position(const Vector2 target_position);
    Model model;
    
private:
    int id;
    const char* name;
    
    EntityTeam team;
    EntityState state;
    
    bool visibility = true;
    bool collision = true;
    Physics::Object* object = nullptr;


    Vector2 target_position;

protected:
    EntityTag tag;
    Vector2 position;
    float rotation;
    Vector2 scale;

    void set_id(int id);
};
