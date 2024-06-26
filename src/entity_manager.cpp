﻿#include <format>
#include <iostream>
#include <string>
#include <ranges>
#include "entity_manager.h"

EntityManager& EntityManager::instance()
{
    static auto instance = EntityManager();
    return instance;
}

void EntityManager::i_update() const
{
    for(const auto& entity : entity_map_ | std::views::values)
    {
        if( !entity ) continue;
        entity->update();
    }
}

void EntityManager::i_draw() const
{
    
    for(const auto& entity : entity_map_ | std::views::values)
    {
        if( !entity ) continue;
            entity->draw();
    }
}

int scroll_offset = 0;
void EntityManager::i_draw_debug(const Camera& camera) const
{
    /*bool alternate = true;
    
    int y_offset = 2 + scroll_offset;

    for( const auto& [id, entity] : entity_map_ )
    {
        constexpr int x_offset = 30;
        if( !entity ) continue;
        alternate = !alternate;
        //entity->draw_debug(camera);
        const Color color = alternate? Color{0,0,0,128}: Color{0,0,0,196};
        auto [mx, my] = GetMousePosition();

        if( mx > 0 && mx < 300
            && my > y_offset -3 && my < y_offset + 13 )
        {
            auto bb = GetMeshBoundingBox(entity->model.meshes[0]);
            BeginMode3D(camera);
            DrawCubeWires(entity->position, 2 * bb.max.x, 2 * bb.max.y, 2 * bb.max.z, RED);
            EndMode3D();
            DrawText(std::format("BB: {}", bb.min.x).c_str(), 350, y_offset, 16, RED);
            DrawRectangle(0, y_offset - 3, 300, 16, RED );
            entity->draw_debug(camera);
        }
        else
        {
            DrawRectangle(0, y_offset - 3, 300, 16, color );
        }
        int length = MeasureText(std::to_string(id).c_str(), 10);
        DrawText( std::to_string(id).c_str(), x_offset - length, y_offset, 10, ORANGE );
        DrawText( entity->get_name(), x_offset + 10, y_offset, 10, GOLD );
        length = MeasureText(entity->get_name(), 10);
        const auto [x, y, z] = entity->get_position();
        const std::string text = "( X: " + std::format("{:.2f}", x) + " Y: " + std::format("{:.2f}", y) + " Z: " + std::format("{:.2f}", z) + " )";
        DrawText(text.c_str(), x_offset + length + 20, y_offset, 10, GOLD );


        y_offset += 16;
    }
    
    if( GetMousePosition().x < 300)
    {
        const int change = GetMouseWheelMove();
        if( change < 0 && y_offset > GetScreenHeight())
            scroll_offset += change * 40;
            
        if( change > 0 && scroll_offset < 0)
            scroll_offset += change * 40;
    }*/
}

int EntityManager::i_get_entity_count() const
{
    return entity_map_.size();
}

Entity* EntityManager::i_spawn_entity(const EntityType type, const Vector3 position = {0, 0, 0}, const Vector3 rotation = {0, 0, 0})
{
    std::cout << "Entity Manager: Spawning entity of type: " << type << ".\n";

    Entity* entity = nullptr;
    if( !entity ) return entity;
    add_entity_to_manager(entity);
    std::cout << "Entity Manager: Entity spawned.\n";

    return entity;
}

void EntityManager::i_add_entity_to_manager( Entity* entity  )
{
    if( !entity ) return;
    std::cout << "Entity Manager: Adding entity with name: " << entity->get_name() << " to entity map.\n";
    // If ID in map increase ID
    while( entity_map_.contains(id_iterator_))
        id_iterator_++;

    // Set ID
    const int id = id_iterator_;
    entity->set_id(id);

    // Add entity to map
    entity_map_[id] = entity;
    
    id_iterator_++;
    
    std::cout << "Entity Manager: Entity with ID: " << entity->get_id() << " added to entity map.\n";

}

void EntityManager::i_remove_entity_from_manager(const Entity* entity  )
{
    if( !entity ) return;
    std::cout << "Entity Manager: Removing entity with ID: " << entity->get_id() << ".\n";
    entity_map_.erase(entity->get_id());
    delete(entity);
    std::cout << "Entity Manager: Entity removed.\n";
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
    for(const auto& tmp_entity : entity_map_ | std::views::values)
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

void EntityManager::i_free()
{
    std::cout << "Entity Manager: Freeing Entity Memory.\n";
    for(const auto& entity : entity_map_ | std::views::values)
    {
        if( !entity ) continue;
        delete entity;
    }
    std::cout << "Entity Manager: Done.\n";
    std::cout << "Entity Manager: Freeing Entity Map.\n";
    entity_map_.clear();
    std::cout << "Entity Manager: Done.\n";
}
