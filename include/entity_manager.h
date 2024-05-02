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
    static int get_entity_count(){ return instance().i_get_entity_count();};
    static Entity* spawn_entity(const EntityType type, const Vector3 position = {0, 0, 0}, const Vector3 rotation = {0, 0, 0}) { return i_spawn_entity(type, position, rotation);}
    static void add_entity_to_manager(Entity* entity) { instance().i_add_entity_to_manager(entity);}
    static void remove_entity_from_manager(const Entity* entity) { instance().i_remove_entity_from_manager(entity);}
    static Entity* get_entity_by_id(const int id) { return instance().i_get_entity_by_id(id);}
    static Entity* get_entity_by_tag(const EntityTag tag) { return instance().i_get_entity_by_tag(tag);}
    static void free(){ instance().i_free(); }

private:
    EntityManager() : id_iterator_(0) {}
    
    void i_update() const;
    void i_draw() const;
    void i_draw_debug(const Camera& camera) const;
    int i_get_entity_count() const;
    static Entity* i_spawn_entity( const EntityType type, Vector3 position, Vector3 rotation );
    
    void i_add_entity_to_manager(Entity* entity);
    void i_remove_entity_from_manager(const Entity* entity);

    Entity* i_get_entity_by_id(int id) const;
    Entity* i_get_entity_by_tag( EntityTag tag) const;

    void i_free();
    
    //       id_   ptr
    std::map<int, Entity*> entity_map_;
    int id_iterator_;
    
};

#endif // ENTITY_MANAGER_H