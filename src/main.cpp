#include <raylib.h>
#include <raymath.h>
#include "collision.h"
#include "entity.h"
#include "f_mortar.h"

int main(void)
{
    // Init window
    SetConfigFlags(FLAG_VSYNC_HINT); 
    constexpr int screen_width = 1280;
    constexpr int screen_height = 720;
    InitWindow(screen_width, screen_height, "Humans Vs Automatons - [raylib]");

    // 3D Camera
    Camera3D camera = {0};
    camera.position = Vector3{ 0.0f, 20.0f, 20.0f }; // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type

    DisableCursor(); // Limit cursor to relative movement inside the window
    FMortar mortar({20,0,20});

    constexpr float fixed_update_interval = 1.0f / 60.0f;
    float fixed_update_accumulator = 0.0f;
    
    while (!WindowShouldClose()) // Main game loop
    {
        // Update
        //-------------------------------------------------------------------------------------
        fixed_update_accumulator += GetFrameTime();
        
        mortar.update();
        
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
            mortar.update_fixed();
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
        int board_rows = 5;
        int board_cols = 12;
        
        for( int i = 0; i < board_rows; i++ )
        {
            float y = static_cast<float>(i * 2 - board_rows + 1);
            for( int j = 0; j < board_cols; j++)
            {
                const Color color = is_gray? GRAY : DARKGRAY;
                float x = static_cast<float>(j * 2 - board_cols + 1);
                Vector3 position = {x, 0, y};
                DrawPlane(position, {2.0f, 2.0f}, color);
                is_gray = !is_gray;
            }
            is_gray = !is_gray;
        }

        // Draw mouse position
        DrawSphere(mouse_world_pos, 0.1f, RED);
        //-------------------------------------------------------------------------------------

        EndMode3D();
        EndDrawing();
    }

    CloseWindow(); // De-Initialization

    return 0;
}
