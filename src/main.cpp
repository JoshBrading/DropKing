#include <format>
#include <raylib.h>
#include <raymath.h>
#include "collision.h"
#include "entity.h"
#include "entity_manager.h"
#include "mem.h"

bool DEBUG = false;

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
    
    MemoryManager::get_usage();
    while (!WindowShouldClose()) // Main game loop
    {

        // Update
        //-------------------------------------------------------------------------------------
        fixed_update_accumulator += GetFrameTime();

        EntityManager::update();
        
        Vector2 mouse_screen_position = GetMousePosition();

        // Convert screen coordinates to world coordinates
        Ray ray = GetMouseRay(mouse_screen_position, camera);
        Vector3 mouse_world_position = Vector3Zero();
        float distance = 0;
        if (ray_intersects_plane(ray, Vector3{0, 0, 0}, Vector3{0, 1, 0}, &distance))
        {
            mouse_world_position = Vector3Add(ray.position, Vector3Scale(ray.direction, distance));
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
        DrawSphere(mouse_world_position, 0.1f, RED);
        //-------------------------------------------------------------------------------------

        EntityManager::draw();
        EndMode3D();
        if( IsKeyPressed(KEY_F1))
        {
            DEBUG = !DEBUG;
        }
        if( DEBUG )
        {
            EntityManager::draw_debug(camera);
            int usage = MemoryManager::get_usage();
            const std::string memory_usage = std::format("Memory Usage: {}/Bytes", usage);
            DrawText(memory_usage.c_str(), 10, GetScreenHeight() - 20, 10, WHITE);
        }
        DrawFPS(20, 20);
        
        //const std::string position = "( X: " + std::format("{:.2f}", mouse_world_position.x) + " Y: " + std::format("{:.2f}", mouse_world_position.y) + " Z: " + std::format("{:.2f}", mouse_world_position.z) + " )";
        //const int length = MeasureText(position.c_str(), 10);
        //DrawText(position.c_str(), static_cast<int>(mouse_screen_position.x) - (length / 2), static_cast<int>(mouse_screen_position.y) + 10, 10, WHITE );
        
        EndDrawing();
    }
    
    EntityManager::free();
    CloseWindow(); // De-Initialization
    MemoryManager::get_usage();

    return 0;
}
