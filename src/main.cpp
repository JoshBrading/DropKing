
#include "raylib.h"
#include <projectile.h>
int main(void)
{
    // Init window
    constexpr int screen_width = 1280;
    constexpr int screen_height = 720;
    InitWindow(screen_width, screen_height, "Humans Vs Automatons - [raylib]");

    // 3D Camera
    Camera3D camera = { 0 };
    camera.position = Vector3{ 10.0f, 10.0f, 10.0f }; // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type


    const Model model = LoadModel("assets/projectile.obj");

    projectile projectile({ 0, 0, 0 }, {10, 30, 0}, model); // Initial position, direction, speed, gravity, and model


    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(120);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FREE);
        projectile.update();
        if (IsKeyPressed('Z')) camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode3D(camera);

        projectile.draw();
        DrawGrid(20, 1.0f);

        EndMode3D();

        DrawRectangle(10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(10, 10, 320, 93, BLUE);

        DrawText("Free camera default controls:", 20, 20, 10, WHITE);
        DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, LIGHTGRAY);
        DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, LIGHTGRAY);
        DrawText("- Z to zoom to (0, 0, 0)", 40, 80, 10, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}