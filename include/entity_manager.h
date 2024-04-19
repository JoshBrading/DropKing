#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H
#include <map>
#include <vector>

#include "entity.h"

class EntityManager
{
public:
    EntityManager();

    void update();
    void draw();

    Entity* get_entity_by_id(int id);
    Entity* get_entity_by_tag(EntityTag tag);
    Entity* get_entity_target_random(EntityTeam team_mask[]);

private:
    //       id_   ptr
    std::map<int, Entity*> entity_map_;
    std::vector<Entity*> entity_vector_;
    
};

#endif // ENTITY_MANAGER_H