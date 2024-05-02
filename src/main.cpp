#include <format>
#include <raylib.h>
#include <raymath.h>
#include "collision.h"
#include "entity.h"
#include "entity_manager.h"
#include "mem.h"
#include "menu.h"

#define RLIGHTS_IMPLEMENTATION
#include <rlights.h>

bool DEBUG = false;
bool SHOULD_CLOSE = false;
bool PAUSE = false;

int main(void)
{
    // Init window
    SetConfigFlags(FLAG_VSYNC_HINT); 
    constexpr int screen_width = 1280;
    constexpr int screen_height = 720;
    InitWindow(screen_width, screen_height, "Humans Vs Automatons - [raylib]");
    SetExitKey(KEY_NULL);
    // 3D Camera
    Camera3D camera;
    camera.position = Vector3{ 0.0f, 20.0f, 20.0f }; // Camera position
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type

    Shader shader = LoadShader(TextFormat("assets/shaders/glsl%i/lighting.vs", 330),
                               TextFormat("assets/shaders/glsl%i/lighting.fs", 330));
    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading, 
    // no need to get the location again if using that uniform name
    //shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");
    
    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(shader, "ambient");
    float ambient[] = {1.1f, 1.1f, 1.1f, 1.0f};
    SetShaderValue(shader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);


    // Create lights
    Light lights[MAX_LIGHTS] = {};
    lights[0] = CreateLight(LIGHT_POINT, { -2, 1, -2 }, Vector3Zero(), YELLOW, shader);
    lights[0].target = {static_cast<float>(GetRandomValue(1, 3)), static_cast<float>(GetRandomValue(0, 3)), static_cast<float>(GetRandomValue(3, 5))};
    lights[1] = CreateLight(LIGHT_POINT, { 2, 1, 2 }, Vector3Zero(), RED, shader);
    lights[1].target = {static_cast<float>(GetRandomValue(1, 3)), static_cast<float>(GetRandomValue(0, 3)), static_cast<float>(GetRandomValue(3, 5))};
    lights[2] = CreateLight(LIGHT_POINT, { -2, 1, 2 }, Vector3Zero(), GREEN, shader);
    lights[2].target = {static_cast<float>(GetRandomValue(1, 3)), static_cast<float>(GetRandomValue(0, 3)), static_cast<float>(GetRandomValue(3, 5))};
    lights[3] = CreateLight(LIGHT_POINT, { 2, 1, -2 }, Vector3Zero(), BLUE, shader);
    lights[3].target = {static_cast<float>(GetRandomValue(1, 3)), static_cast<float>(GetRandomValue(0, 3)), static_cast<float>(GetRandomValue(3, 5))};

    //DisableCursor(); // Limit cursor to relative movement inside the window

   //  Spawn an FMortar entity
    EntityManager::spawn_entity(EntityType::MORTAR)->model.materials->shader = shader;
    EntityManager::spawn_entity(EntityType::MORTAR)->model.materials->shader = shader;
    EntityManager::spawn_entity(EntityType::MORTAR)->model.materials->shader = shader;
    EntityManager::spawn_entity(EntityType::MORTAR)->model.materials->shader = shader;

    constexpr float fixed_update_interval = 1.0f / 60.0f;
    float fixed_update_accumulator = 0.0f;
    constexpr int board_rows = 7;

    for( int i = 0; i < board_rows; i++ )
    {
        constexpr int board_cols = 12;
        float y = static_cast<float>(i * 2 - board_rows + 1);
        for( int j = 0; j < board_cols; j++)
        {
            float x = static_cast<float>(j * 2 - board_cols + 1);
            const Vector3 position = {x, 0, y};
            auto ent = EntityManager::spawn_entity(TILE, position, {0, static_cast<float>(i + j) * 45.0f, 0});
            ent->model.materials[0].shader = shader;
        }
    }
    

    Menu menu;
    menu.add_label("Menu Title", GetFontDefault(), 48, {100, 100});
    menu.add_label("Menu Description", GetFontDefault(), 16, {100, 150});
    menu.add_button("button 1",GetFontDefault(), {100, 200}, 100, 10, nullptr, nullptr);
    menu.add_button("button 2",GetFontDefault(), {100, 230}, 100, 10, nullptr, nullptr);
    menu.add_button("button 3",GetFontDefault(), {100, 260}, 100, 10, nullptr, nullptr);
    menu.add_button("button 4",GetFontDefault(), {100, 290}, 100, 10, nullptr, nullptr);
    menu.add_button("button 5",GetFontDefault(), {100, 320}, 100, 10, nullptr, nullptr);
    menu.add_button("button 6",GetFontDefault(), {100, 350}, 100, 10, nullptr, nullptr);
    menu.add_button("Quit",GetFontDefault(), {100, static_cast<float>(GetScreenHeight()) - 50.0f}, 100, 10, [](Menu*, void*){SHOULD_CLOSE = !SHOULD_CLOSE;}, nullptr);

    Menu store;
    store.add_button("Toggle Debug", GetFontDefault(), {50, 50}, 100, 10, [](Menu*, void*){DEBUG = !DEBUG;}, nullptr)->is_toggle = true;
    store.add_button("Show Pause Menu", GetFontDefault(), {50, 80}, 100, 10, [&menu](Menu*, void*){PAUSE = !PAUSE; menu.toggle();}, nullptr);
    store.add_button("Pause Updates", GetFontDefault(), {50, 110}, 100, 10, [](Menu*, void*){PAUSE = !PAUSE;}, nullptr)->is_toggle = true;
    store.add_button("Spawn Missile", GetFontDefault(), {275, 50}, 100, 10, nullptr, nullptr);
    store.add_button("Spawn Turret", GetFontDefault(), {500, 50}, 100, 10, nullptr, nullptr);
    store.toggle();
    store.darken_background = false;
    
    MemoryManager::get_usage();
    while (!SHOULD_CLOSE) // Main game loop
    {
        UpdateCamera(&camera, CAMERA_ORBITAL);
        // Update
        //-------------------------------------------------------------------------------------
        fixed_update_accumulator += GetFrameTime();

        if(!PAUSE)
        {
            EntityManager::update();
            

            for(auto& light : lights)
            {
                const float distance = Vector3Distance(light.position, light.target);
                if(distance > 0.2f)
                {
                    const Vector3 direction = Vector3Normalize(Vector3Subtract(light.target, light.position));
                    light.position = Vector3Add(light.position, Vector3Scale(direction, 10.0f * GetFrameTime()));
                }
                else
                {
                    light.target = {static_cast<float>(GetRandomValue(-10, 10)), static_cast<float>(GetRandomValue(-10, 10)), static_cast<float>(GetRandomValue(-10, 10))};
                }
            }
        }
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        
        // Check key inputs to enable/disable lights
        if (IsKeyPressed(KEY_Y)) { lights[0].enabled = !lights[0].enabled; }
        if (IsKeyPressed(KEY_R)) { lights[1].enabled = !lights[1].enabled; }
        if (IsKeyPressed(KEY_G)) { lights[2].enabled = !lights[2].enabled; }
        if (IsKeyPressed(KEY_B)) { lights[3].enabled = !lights[3].enabled; }
        
        // Update light values (actually, only enable/disable them)
        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);
        
        Vector2 mouse_screen_position = GetMousePosition();
        
        // Convert screen coordinates to world coordinates
        Ray ray = GetMouseRay(mouse_screen_position, camera);
        Vector3 mouse_world_position = Vector3Zero();
        float distance = 0;
        if (ray_intersects_plane(ray, Vector3{0, 0, 0}, Vector3{0, 1, 0}, &distance))
        {
            mouse_world_position = Vector3Add(ray.position, Vector3Scale(ray.direction, distance));
        }
        
        store.update();
        menu.update();
        //-------------------------------------------------------------------------------------
        
        // Run Fixed Updates
        //-------------------------------------------------------------------------------------
        if( fixed_update_accumulator >= fixed_update_interval )
        {
            store.update_fixed();
            menu.update_fixed();

            fixed_update_accumulator -= fixed_update_interval;
        }
        //-------------------------------------------------------------------------------------
        
        // Draw
        //-------------------------------------------------------------------------------------

        BeginDrawing();
            ClearBackground(DARKGRAY);
            BeginMode3D(camera);
                BeginShaderMode(shader);
                    // Draw mouse position
                    DrawSphere(mouse_world_position, 0.1f, RED);
                    //-------------------------------------------------------------------------------------

                    EntityManager::draw();
                EndShaderMode();
        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
            else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
        }


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
            store.draw();
            menu.draw();
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
