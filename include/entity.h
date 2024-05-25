#ifndef ENTITY_H
#define ENTITY_H
#include <raylib.h>

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
    CHARACTER,
    NONE
};

class Entity
{
    friend class EntityManager;
public:
    Entity(const Vector3 position, const Vector3 rotation, const char* model_path, const char* name);
    Entity(Entity&&) = delete; // Move constructor
    Entity(const Entity&) = delete; // Copy constructor
    Entity& operator=(Entity&&) = delete; // Move assignment operator
    Entity& operator=(const Entity&) = delete; // Copy assignment operator
    virtual ~Entity(); // Destructor, needed for virtual function overrides?
    
    virtual void update();
    virtual void update_fixed();
    virtual void draw();
    virtual void draw_debug(const Camera& camera);

    int get_id() const;
    const char* get_name() const;
    
    EntityTag get_tag() const;
    void set_tag(EntityTag tag);
    
    EntityTeam get_team() const;
    void set_team(EntityTeam team);

    Vector3 get_position() const;
    
    Vector3 get_target_position() const;
    void set_target_position( Vector3 target_position );
    Model model;
    
private:
    int id;
    const char* name;
    
    EntityTag tag;
    EntityTeam team;
    EntityState state;
    
    bool visibility;
    bool collision;


    Vector3 target_position;

protected:
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;

    // 2D Conversion
    Vector2 position_2d;
    
    void set_id(int id);
};

#endif //ENTITY_H
