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

#include "player.h"

bool DEBUG = false;
bool DEBUG_FOOTER = false;
bool SHOULD_CLOSE = false;
bool PAUSE = false;

int main(void)
{
    // Init window
    //SetConfigFlags(FLAG_VSYNC_HINT); 
    constexpr int screen_width = 1280;
    constexpr int screen_height = 720;
    InitWindow(screen_width, screen_height, "DropKing - [raylib]");
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

    
    SetTargetFPS(60);
    // Create lights
    /*Light lights[MAX_LIGHTS] = {};
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
    EntityManager::spawn_entity(EntityType::MORTAR)->model.materials->shader = shader;*/

    constexpr float fixed_update_interval = 1.0f / 60.0f;
    float fixed_update_accumulator = 0.0f;
    /*constexpr int board_rows = 7;

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
    }*/
    

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
    store.add_label("Debug", GetFontDefault(), 24, {GetScreenWidth() - 210.0f, 50.0f});
    Menu debug_submenu;
    debug_submenu.darken_background = false;
    debug_submenu.add_button("Show Entity List", GetFontDefault(), {GetScreenWidth() - 415.0f, 75}, 100, 10, [](Menu*, void*){DEBUG = !DEBUG;}, nullptr)->is_toggle = true;
    debug_submenu.add_button("Show Footer", GetFontDefault(), {GetScreenWidth() - 415.0f, 100}, 100, 10, [](Menu*, void*){DEBUG_FOOTER = !DEBUG_FOOTER;}, nullptr)->is_toggle = true;
    debug_submenu.add_button("Pause Updates", GetFontDefault(), {GetScreenWidth() - 415.0f, 125}, 100, 10, [](Menu*, void*){PAUSE = !PAUSE;}, nullptr)->is_toggle = true;
    debug_submenu.add_button("Toggle Pause Menu", GetFontDefault(), {GetScreenWidth() - 415.0f, 150}, 100, 10, [&menu](Menu*, void*){PAUSE = !PAUSE; menu.toggle();}, nullptr);

    store.add_button("Debug", GetFontDefault(), {GetScreenWidth() - 210.0f, 75}, 100, 10, [&debug_submenu, &store](Menu*, void*){ store.is_focused = true; debug_submenu.toggle();}, nullptr);
    store.add_label("Gameplay", GetFontDefault(), 24, {GetScreenWidth() - 210.0f, 150.0f});
    store.add_button("Spawn Missile", GetFontDefault(), {GetScreenWidth() - 210.0f, 175}, 100, 10, nullptr, nullptr);
    store.add_button("Spawn Turret", GetFontDefault(), {GetScreenWidth() - 210.0f, 200}, 100, 10, nullptr, nullptr);
    store.toggle();
    store.darken_background = false;


    
    Polygon* MOCK;
    
    Body* c2;
    
    MOCK = new Polygon();
    MOCK->origin = {250, 450};
    MOCK->points.push_back({200, 400});
    MOCK->points.push_back({200, 500});
    MOCK->points.push_back({300, 500});
    MOCK->points.push_back({300, 400});
    
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    
    Polygon* roof = new Polygon();
    Body* c_roof = new Body(roof);
    roof->origin = {width / 2.0f, -10};
    roof->points.push_back({-20, -20});
    roof->points.push_back({(float)width + 20, -20});
    roof->points.push_back({(float)width + 20, 5});
    roof->points.push_back({-20, 5});
    c_roof->is_static = true;
    c_roof->parent = nullptr;
    c_roof->type = POLYGON;
    c_roof->polygon = roof;
    c_roof->rotation_static = true;
    Collision::add_collider(c_roof);

    Polygon* floor = new Polygon();
    Body* c_floor = new Body(floor);
    floor->origin = {width / 2.0f, (float)height + 10};
    floor->points.push_back({-20, (float)height + 20});
    floor->points.push_back({(float)width + 20, (float)height + 20});
    floor->points.push_back({(float)width + 20, (float)height - 5});
    floor->points.push_back({-20, (float)height - 5});
    c_floor->is_static = true;
    c_floor->parent = nullptr;
    c_floor->type = POLYGON;
    c_floor->polygon = floor;
    c_floor->rotation_static = true;
    Collision::add_collider(c_floor);

    Polygon* left_wall = new Polygon();
    Body* c_left_wall = new Body(left_wall);
    left_wall->origin = {-10, height / 2.0f};
    left_wall->points.push_back({-20, -20});
    left_wall->points.push_back({5, -20});
    left_wall->points.push_back({5, (float)height + 20});
    left_wall->points.push_back({-20, (float)height + 20});
    c_left_wall->is_static = true;
    c_left_wall->parent = nullptr;
    c_left_wall->type = POLYGON;
    c_left_wall->polygon = left_wall;
    c_left_wall->rotation_static = true;
    Collision::add_collider(c_left_wall);

    Polygon* right_wall = new Polygon();
    Body* c_right_wall = new Body(right_wall);
    right_wall->origin = {(float)width + 10, height / 2.0f};
    right_wall->points.push_back({(float)width + 20, -20});
    right_wall->points.push_back({(float)width - 5, -20});
    right_wall->points.push_back({(float)width - 5, (float)height + 20});
    right_wall->points.push_back({(float)width + 20, (float)height + 20});
    c_right_wall->is_static = true;
    c_right_wall->parent = nullptr;
    c_right_wall->type = POLYGON;
    c_right_wall->polygon = right_wall;
    c_right_wall->rotation_static = true;
    Collision::add_collider(c_right_wall);

    Polygon* ramp = new Polygon();
    Body* c_ramp = new Body(ramp);
    ramp->origin = {600, 350};
    ramp->points.push_back({400, 500});
    ramp->points.push_back({800, 200});
    ramp->points.push_back({800, 300});
    ramp->points.push_back({400, 600});
    c_ramp->is_static = true;
    c_ramp->parent = nullptr;
    c_ramp->type = POLYGON;
    c_ramp->polygon = ramp;
    c_ramp->rotation_static = false;
    //Collision::add_collider(c_ramp);
    
    c2 = new Body(MOCK);
    c2->is_static = false;
    c2->parent = nullptr;
    c2->type = POLYGON;
    c2->polygon = MOCK;
    Collision::add_collider(c2);

    Polygon* octogon = new Polygon();
    Body* c_octogon = new Body(octogon);
    octogon->origin = {800, 400};
    octogon->points.push_back({775, 350});
    octogon->points.push_back({825, 350});
    octogon->points.push_back({850, 375});
    octogon->points.push_back({850, 425});
    octogon->points.push_back({825, 450});
    octogon->points.push_back({775, 450});
    octogon->points.push_back({750, 425});
    octogon->points.push_back({750, 375});
    c_octogon->is_static = false;
    c_octogon->parent = nullptr;
    c_octogon->type = POLYGON;
    c_octogon->polygon = octogon;
    Collision::add_collider(c_octogon);

    c2->angular_velocity += 100;
    
    Player player({300, 0}, 100, 100);
    MemoryManager::get_usage();
    while (!SHOULD_CLOSE) // Main game loop
    {
        //UpdateCamera(&camera, CAMERA_FREE);
        // Update
        //-------------------------------------------------------------------------------------
        fixed_update_accumulator += GetFrameTime();

        if(!PAUSE)
        {
            EntityManager::update();
            
        }
        
        Vector2 mouse_screen_position = GetMousePosition();
        
        // Convert screen coordinates to world coordinates
        Ray ray = GetMouseRay(mouse_screen_position, camera);
        Vector3 mouse_world_position = Vector3Zero();
        float distance = 0;
        
        store.update();
        debug_submenu.update();
        menu.update();
        
        player.update();
        //-------------------------------------------------------------------------------------
        
        // Run Fixed Updates
        //-------------------------------------------------------------------------------------
        if( fixed_update_accumulator >= fixed_update_interval && !PAUSE)
        {
            store.update_fixed();
            debug_submenu.update_fixed();
            menu.update_fixed();

                c2->angular_velocity = -75;
            
            fixed_update_accumulator -= fixed_update_interval;
        }
        //-------------------------------------------------------------------------------------
        Collision::update(1);
        Collision::draw_debug();
        // Draw
        //-------------------------------------------------------------------------------------

        BeginDrawing();
            ClearBackground(DARKGRAY);
            //BeginMode3D(camera);
                BeginShaderMode(shader);
                    // Draw mouse position
                    DrawSphere(mouse_world_position, 0.1f, RED);
                    //-------------------------------------------------------------------------------------

                    EntityManager::draw();
                EndShaderMode();

            player.draw();
        
            //EndMode3D();
            if( IsKeyPressed(KEY_F1))
            {
                DEBUG = !DEBUG;
            }
            if( DEBUG )
            {
                EntityManager::draw_debug(camera);
            }
            if( DEBUG_FOOTER )
            {
                int usage = MemoryManager::get_usage();
                const std::string memory_usage = std::format("Memory Usage: {}/Bytes", usage);
                DrawRectangle(0, GetScreenHeight() - 24, GetScreenWidth(), 24, BLACK);
                DrawText(memory_usage.c_str(), 10, GetScreenHeight() - 16, 10, WHITE);
                const std::string entity_count = std::format("Entity Count: {}", EntityManager::get_entity_count());
                DrawText(entity_count.c_str(), 200, GetScreenHeight() - 16, 10, WHITE);
            }
        
            DrawFPS(20, 20);
            //store.draw();
            //debug_submenu.draw();
            //menu.draw();
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
