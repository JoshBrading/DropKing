#include "entity_manager.h"

#include <format>

#include "f_mortar.h"
#include <iostream>
#include <string>


EntityManager& EntityManager::instance()
{
    static auto instance = EntityManager();
    return instance;
}

void EntityManager::i_update() const
{
    for( const auto& [_, entity] : entity_map_ )
    {
        if( !entity ) continue;
        entity->update();
    }
}

void EntityManager::i_draw() const
{
    
    for( const auto& [_, entity] : entity_map_ )
    {
        if( !entity ) continue;
            entity->draw();
    }
}

void EntityManager::i_draw_debug(const Camera& camera) const
{
    int x = 100;
    int y = 100;
    for( const auto& [id, entity] : entity_map_ )
    {
        if( !entity ) continue;

        entity->draw_debug(camera);
        int length = MeasureText(std::to_string(id).c_str(), 10);
        DrawText( std::to_string(id).c_str(), x - length, y, 10, ORANGE );
        DrawText( entity->get_name(), x + 10, y, 10, GOLD );
        
        const auto position = entity->get_position();
        const std::string text = "( X: " + std::format("{:.2f}", position.x) + " Y: " + std::format("{:.2f}", position.y) + " Z: " + std::format("{:.2f}", position.z) + " )";
        length = MeasureText(text.c_str(), 10);
        DrawText(text.c_str(), x + 50, y, 10, GOLD );
        
        y += 15;
    }
}

Entity* EntityManager::i_spawn_entity(const EntityType type)
{
    Entity* entity = nullptr;
    switch( type )
    {
        case MORTAR:
            entity = new FMortar({0,0,0});
            break;
        default:
            return entity;
    }

    add_entity_to_manager(entity);
    
    return entity;
}

void EntityManager::i_add_entity_to_manager( Entity* entity  )
{
    if( !entity ) return;
    
    // If ID in map increase ID
    while( entity_map_.contains(id_iterator_))
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

void EntityManager::i_remove_entity_from_manager(const Entity* entity  )
{
    if( !entity ) return;
    entity_map_.erase(entity->get_id());
    std::cout << "|----------------------------------------------------\n";
    std::cout << "| Entity Manager: Entity with ID: " << entity->get_id() << " removed.\n";
    std::cout << "|----------------------------------------------------\n";
}

Entity* EntityManager::i_get_entity_by_id(const int id) const
{
    Entity* entity = nullptr;
    if( entity_map_.contains(id))
        entity = entity_map_.find(id)->second;
    return entity;
}

Entity* EntityManager::i_get_entity_by_tag(const EntityTag tag) const
{
    Entity* entity = nullptr; 
    for( const auto& [_, tmp_entity] : entity_map_ )
    {
        if( !tmp_entity ) continue;
        if( tmp_entity->get_tag() == tag )
        {
            entity = tmp_entity;
            break;
        }
    }
    return entity;
}
