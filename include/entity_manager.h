#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H
#include <map>
#include <raylib.h>

#include "entity.h"

class EntityManager
{
public:
    static EntityManager& instance();
    static void update() { instance().i_update();}
    static void draw() { instance().i_draw();}
    static void draw_debug(const Camera& camera) { instance().i_draw_debug(camera);}
    static Entity* spawn_entity(const EntityType type) { return instance().i_spawn_entity(type);}
    static void add_entity_to_manager(Entity* entity) { instance().i_add_entity_to_manager(entity);}
    static void remove_entity_from_manager(Entity* entity) { instance().i_remove_entity_from_manager(entity);}
    static Entity* get_entity_by_id(const int id) { return instance().i_get_entity_by_id(id);}
    static Entity* get_entity_by_tag(const EntityTag tag) { return instance().i_get_entity_by_tag(tag);}
private:
    EntityManager() : id_iterator_(0) {}
    
    void i_update() const;
    void i_draw() const;
    void i_draw_debug(const Camera& camera) const;

    Entity* i_spawn_entity( const EntityType type );
    
    void i_add_entity_to_manager(Entity* entity);
    void i_remove_entity_from_manager(const Entity* entity);

    Entity* i_get_entity_by_id(int id) const;
    Entity* i_get_entity_by_tag( EntityTag tag) const;
    
    //       id_   ptr
    std::map<int, Entity*> entity_map_;
    int id_iterator_;
    
};

#endif // ENTITY_MANAGER_H