#include <format>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "collision.h"
#include "entity.h"
#include "entity_manager.h"

struct MemoryAllocation
{
    int total_allocated = 0;
    int total_free = 0;
    void print_usage() const
    {
        std::cout << "Memory Used: " << total_allocated - total_free << " bytes.\n";
    }
};

MemoryAllocation MEMORY_ALLOCATION;

// ReSharper disable CppParameterNamesMismatch
void* operator new(const size_t size)  // NOLINT(misc-new-delete-overloads)
{
    MEMORY_ALLOCATION.total_allocated += static_cast<int>(size);
    return malloc(size);
}

void operator delete(void* memory, const size_t size)  // NOLINT(misc-new-delete-overloads, clang-diagnostic-implicit-exception-spec-mismatch)
{
    MEMORY_ALLOCATION.total_free += static_cast<int>(size);
    free(memory);
}
// ReSharper restore CppParameterNamesMismatch


int main(void)
{
    // Init window
    SetConfigFlags(FLAG_VSYNC_HINT); 
    constexpr int screen_width = 1280;
    constexpr int screen_height = 720;
    InitWindow(screen_width, screen_height, "Humans Vs Automatons - [raylib]");

    // 3D Camera
    Camera3D camera;
    camera.position = Vector3{ 0.0f, 20.0f, 20.0f }; // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type

    DisableCursor(); // Limit cursor to relative movement inside the window

   //  Spawn an FMortar entity
    EntityManager::spawn_entity(EntityType::MORTAR);
    EntityManager::spawn_entity(EntityType::MORTAR);
    const Entity* mortar_entity = EntityManager::spawn_entity(EntityType::MORTAR);
    EntityManager::remove_entity_from_manager(mortar_entity);
    mortar_entity = EntityManager::spawn_entity(EntityType::MORTAR);
    EntityManager::remove_entity_from_manager(mortar_entity);
    mortar_entity = EntityManager::spawn_entity(EntityType::MORTAR);
    EntityManager::remove_entity_from_manager(mortar_entity);
    EntityManager::spawn_entity(EntityType::MORTAR);
    EntityManager::spawn_entity(EntityType::MORTAR);
    constexpr float fixed_update_interval = 1.0f / 60.0f;
    float fixed_update_accumulator = 0.0f;
    
    while (!WindowShouldClose()) // Main game loop
    {

        // Update
        //-------------------------------------------------------------------------------------
        fixed_update_accumulator += GetFrameTime();

        EntityManager::update();
        
        Vector2 mouse_screen_pos = GetMousePosition();

        // Convert screen coordinates to world coordinates
        Ray ray = GetMouseRay(mouse_screen_pos, camera);
        Vector3 mouse_world_pos = Vector3Zero();
        float distance = 0;
        if (ray_intersects_plane(ray, Vector3{0, 0, 0}, Vector3{0, 1, 0}, &distance))
        {
            mouse_world_pos = Vector3Add(ray.position, Vector3Scale(ray.direction, distance));
        }

        //-------------------------------------------------------------------------------------
        
        // Run Fixed Updates
        //-------------------------------------------------------------------------------------
        while( fixed_update_accumulator >= fixed_update_interval )
        {
            //mortar_entity->update_fixed();
            fixed_update_accumulator -= fixed_update_interval;
        }
        //-------------------------------------------------------------------------------------
        
        // Draw
        //-------------------------------------------------------------------------------------

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode3D(camera);

        // Draw Game Board
        bool is_gray = true;
        constexpr int board_rows = 5;

        for( int i = 0; i < board_rows; i++ )
        {
            constexpr int board_cols = 12;
            float y = static_cast<float>(i * 2 - board_rows + 1);
            for( int j = 0; j < board_cols; j++)
            {
                const Color color = is_gray? GRAY : DARKGRAY;
                float x = static_cast<float>(j * 2 - board_cols + 1);
                const Vector3 position = {x, 0, y};
                DrawPlane(position, {2.0f, 2.0f}, color);
                is_gray = !is_gray;
            }
            is_gray = !is_gray;
        }

        // Draw mouse position
        DrawSphere(mouse_world_pos, 0.1f, RED);
        //-------------------------------------------------------------------------------------

        EntityManager::draw();
        EndMode3D();
        EntityManager::draw_debug(camera);
        DrawFPS(20, 20);
        
        //const std::string position = "( X: " + std::format("{:.2f}", mouse_world_pos.x) + " Y: " + std::format("{:.2f}", mouse_world_pos.y) + " Z: " + std::format("{:.2f}", mouse_world_pos.z) + " )";
        //const int length = MeasureText(position.c_str(), 10);
        //DrawText(position.c_str(), static_cast<int>(mouse_screen_pos.x) - (length / 2), static_cast<int>(mouse_screen_pos.y) + 10, 10, WHITE );
        
        EndDrawing();
    }
    
    EntityManager::free();
    CloseWindow(); // De-Initialization
    MEMORY_ALLOCATION.print_usage();

    return 0;
}
