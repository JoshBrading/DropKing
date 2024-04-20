#include "entity_manager.h"
#include "f_mortar.h"
#include <iostream>
#include <string>


EntityManager* EntityManager::instance()
{
    static EntityManager* instance = new EntityManager();
    return instance;
}

void EntityManager::update() const
{
    for( const auto& id_entity_pair : entity_map_ )
    {
        Entity* entity = id_entity_pair.second;
        if( !entity ) continue;
        entity->update();
    }
}

void EntityManager::draw() const
{
    
    for( const auto& id_entity_pair : entity_map_ )
    {
        Entity* entity = id_entity_pair.second;
        if( !entity ) continue;
            entity->draw();
    }
}

void EntityManager::draw_debug(const Camera& camera) const
{
    int x = 100;
    int y = 100;
    for( const auto& id_entity_pair : entity_map_ )
    {
        Entity* entity = id_entity_pair.second;
        if( !entity ) continue;

        entity->draw_debug(camera);
        const int length = MeasureText(std::to_string(id_entity_pair.first).c_str(), 10);
        DrawText( std::to_string(id_entity_pair.first).c_str(), x - length, y, 10, ORANGE );
        DrawText( id_entity_pair.second->get_name(), x + 10, y, 10, GOLD );

        y += 10;
    }
}

Entity* EntityManager::spawn_entity(const EntityType type)
{
    Entity* entity = nullptr;
    switch( type )
    {
    case MORTAR:
            entity = new FMortar({0,0,0});
            break;
        case SOLDIER:
            break;
        default:
            return entity;
    }

    add_entity_to_manager(entity);
    
    return entity;
}

void EntityManager::add_entity_to_manager( Entity* entity  )
{
    if( !entity ) return;
    
    // If ID in map increase ID
    while( entity_map_.count(id_iterator_) > 0 )
        id_iterator_++;

    // Set ID
    const int id = id_iterator_;
    entity->set_id(id);

    // Add entity to map
    entity_map_[id] = entity;
    
    id_iterator_++;
    
    std::cout << "|----------------------------------------------------\n";
    std::cout << "| Entity Manager: Entity with ID: " << id << " added.\n";
    std::cout << "|----------------------------------------------------\n";
}

void EntityManager::remove_entity_from_manager(const Entity* entity  )
{
    if( !entity ) return;
    entity_map_.erase(entity->get_id());
    std::cout << "|----------------------------------------------------\n";
    std::cout << "| Entity Manager: Entity with ID: " << entity->get_id() << " removed.\n";
    std::cout << "|----------------------------------------------------\n";
}

Entity* EntityManager::get_entity_by_id(const int id) const
{
    Entity* entity = nullptr;
    if( entity_map_.count(id) > 0 )
        entity = entity_map_.find(id)->second;
    return entity;
}

Entity* EntityManager::get_entity_by_tag(const EntityTag tag) const
{
    Entity* entity = nullptr; 
    for( const auto& id_entity_pair : entity_map_ )
    {
        Entity* tmp_ent = id_entity_pair.second;
        if( !tmp_ent ) continue;
        if( tmp_ent->get_tag() == tag )
        {
            entity = tmp_ent;
            break;
        }
    }
    return entity;
}
