#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H
#include <map>
#include <raylib.h>

#include "entity.h"

class EntityManager
{
public:
    static EntityManager* instance();
    void update() const;
    void draw() const;
    void draw_debug(const Camera& camera) const;

    Entity* spawn_entity( EntityType type );
    
    void add_entity_to_manager(Entity* entity);
    void remove_entity_from_manager(const Entity* entity);

    Entity* get_entity_by_id(int id) const;
    Entity* get_entity_by_tag(EntityTag tag) const;

private:
    EntityManager() : id_iterator_(0) {}
    
    //       id_   ptr
    std::map<int, Entity*> entity_map_;
    int id_iterator_;
    
};

#endif // ENTITY_MANAGER_H