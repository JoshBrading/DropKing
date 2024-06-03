#include <format>
#include <raylib.h>

#include "entity_manager.h"
#include "mem.h"
#include "menu.h"
#include <chipmunk/chipmunk.h>
#include <nlohmann/json.hpp>

#include "editor.h"
#include "game.h"
#include "physics.h"

std::vector<Physics::Object*> OBJECTS;

cpConstraint* makePivot(int o, cpVect pos)
{
    OBJECTS[o]->type = Physics::Shapes::BOX;
    OBJECTS[o]->size = {96, 8};
    OBJECTS[o]->body = cpSpaceAddBody(Physics::Instances::SPACE, cpBodyNew(0, 0));
    OBJECTS[o]->shape = cpSpaceAddShape(Physics::Instances::SPACE, cpBoxShapeNew(OBJECTS[o]->body, OBJECTS[o]->size.x, OBJECTS[o]->size.y, 0.0));
    cpShapeSetFriction(OBJECTS[o]->shape, 0.6);
    cpShapeSetMass(OBJECTS[o]->shape, 8);

    OBJECTS[o + 1]->type = Physics::Shapes::BOX;
    OBJECTS[o + 1]->size = {8, 96};
    OBJECTS[o + 1]->body = OBJECTS[o]->body;
    OBJECTS[o + 1]->shape = cpSpaceAddShape(Physics::Instances::SPACE, cpBoxShapeNew(OBJECTS[o]->body, OBJECTS[o + 1]->size.x, OBJECTS[o + 1]->size.y, 0.0));
    cpShapeSetFriction(OBJECTS[o + 1]->shape, 0.6);
    cpShapeSetMass(OBJECTS[o + 1]->shape, 8);

    cpBodySetPosition(OBJECTS[o]->body, pos);
    return cpSpaceAddConstraint(Physics::Instances::SPACE, cpPivotJointNew(OBJECTS[o]->body, cpSpaceGetStaticBody(Physics::Instances::SPACE), pos));
}

void postSolve(const cpArbiter* arb, const cpSpace* space, const cpDataPointer data)
{
    (void)space;
    (void)data;
    if (cpArbiterIsFirstContact(arb))
    {
        DrawText("Collision", 10, 10, 20, WHITE);
        float f = static_cast<float>(cpArbiterTotalKE(arb));
        if (f > 20000)
        {
            f = 20000;
        }
    }
}

bool DEBUG = false;
bool DEBUG_FOOTER = false;
bool SHOULD_CLOSE = false;
bool PAUSE = true;

int SCORE = 0;

int main(void)
{
    // Init window
    //SetConfigFlags(FLAG_VSYNC_HINT); 
    constexpr int screen_width = 1400;
    constexpr int screen_height = 1000;
    InitWindow(screen_width, screen_height, "DropKing - [raylib]");
    SetExitKey(KEY_NULL);

    Camera2D camera = {0};
    //camera.target = {screen_width / 2.0, screen_height / 2.0};
    camera.offset = {screen_width / 2, screen_height / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

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

    constexpr float fixed_update_interval = 1.0f / 60.0f;
    float fixed_update_accumulator = 0.0f;

    Game::GameWorld* game = new Game::GameWorld();
    Editor* editor = new Editor(&camera, game);

    Menu menu;
    menu.add_label("Menu Title", GetFontDefault(), 48, {100, 100});
    menu.add_label("Menu Description", GetFontDefault(), 16, {100, 150});
    menu.add_button("button 1", GetFontDefault(), {100, 200}, 100, 10, nullptr, nullptr);
    menu.add_button("button 2", GetFontDefault(), {100, 230}, 100, 10, nullptr, nullptr);
    menu.add_button("button 3", GetFontDefault(), {100, 260}, 100, 10, nullptr, nullptr);
    menu.add_button("button 4", GetFontDefault(), {100, 290}, 100, 10, nullptr, nullptr);
    menu.add_button("button 5", GetFontDefault(), {100, 320}, 100, 10, nullptr, nullptr);
    menu.add_button("button 6", GetFontDefault(), {100, 350}, 100, 10, nullptr, nullptr);
    menu.add_button("Quit", GetFontDefault(), {100, static_cast<float>(GetScreenHeight()) - 50.0f}, 100, 10,
                    [](Menu*, void*) { SHOULD_CLOSE = !SHOULD_CLOSE; }, nullptr);

    Menu store;
    store.add_label("Debug", GetFontDefault(), 24, {GetScreenWidth() - 210.0f, 50.0f});
    Menu debug_submenu;
    debug_submenu.darken_background = false;
    debug_submenu.add_button("Show Entity List", GetFontDefault(), {GetScreenWidth() - 415.0f, 75}, 100, 10,
                             [](Menu*, void*) { DEBUG = !DEBUG; }, nullptr)->is_toggle = true;
    debug_submenu.add_button("Show Footer", GetFontDefault(), {GetScreenWidth() - 415.0f, 100}, 100, 10,
                             [](Menu*, void*) { DEBUG_FOOTER = !DEBUG_FOOTER; }, nullptr)->is_toggle = true;
    debug_submenu.add_button("Pause Updates", GetFontDefault(), {GetScreenWidth() - 415.0f, 125}, 100, 10,
                             [](Menu*, void*) { PAUSE = !PAUSE; }, nullptr)->is_toggle = true;
    debug_submenu.add_button("Start Editor", GetFontDefault(), {GetScreenWidth() - 415.0f, 150}, 100, 10,
                             [editor](Menu*, void* data)
                             {
                                 editor->start();
                             }, nullptr);

    store.add_button("Debug", GetFontDefault(), {GetScreenWidth() - 210.0f, 75}, 100, 10,
                     [&debug_submenu, &store](Menu*, void*)
                     {
                         store.is_focused = true;
                         debug_submenu.toggle();
                     }, nullptr);
    store.add_label("Gameplay", GetFontDefault(), 24, {GetScreenWidth() - 210.0f, 150.0f});
    store.add_button("Spawn Missile", GetFontDefault(), {GetScreenWidth() - 210.0f, 175}, 100, 10, nullptr, nullptr);
    store.add_button("Spawn Turret", GetFontDefault(), {GetScreenWidth() - 210.0f, 200}, 100, 10, nullptr, nullptr);
    store.darken_background = false;
    store.toggle();

    // frame counter
    int frame = 0;

    Vector2 mouse = {0.0f, 0.0f};
    cpSpaceSetGravity(Physics::Instances::SPACE, Physics::GRAVITY);

    cpCollisionHandler* handler = cpSpaceAddCollisionHandler(Physics::Instances::SPACE, 0, 0);
    handler->postSolveFunc = reinterpret_cast<cpCollisionPostSolveFunc>(postSolve);

    for (int n = 0; n < Physics::MAX_OBJECTS; n++)
    {
        cpFloat mass;
        Physics::Object* obj;
        //if (n % 2)
        //{
            obj = Physics::create_square({(float)GetRandomValue(12, 48), (float)GetRandomValue(12, 14)}, {(float)GetRandomValue(48, 48), (float)GetRandomValue(48, 48)});
            cpShapeSetFriction(obj->shape, 0);
            OBJECTS.push_back(obj);
    }
    game->load_levels();
    if( Game::Level* level = game->get_active_level() )
        cpBodySetPosition(OBJECTS.back()->body, cpv(level->spawn_point.x, level->spawn_point.y));

    Menu* main_menu = new Menu();
    Menu* level_selector = new Menu();
    int level_count = 0;
    level_selector->add_button("-- Refresh --", GetFontDefault(), {315, 225}, 100, 10, [game, level_selector, &level_count, main_menu, editor](Menu*, void* data)
    {
        std::vector<Game::Level*> levels = game->get_levels();
        if( levels.size() > level_count )
        {
            auto level = levels.back();
            float x = 315;
            float y = 250 + 25 * level_count;
            level_selector->add_button(std::to_string(level_count + 1), GetFontDefault(), {x, y}, 100, 10, [level, game, main_menu, level_selector, editor](Menu*, void* data)
        {
            game->start_level(level);
            cpBodySetPosition(OBJECTS.back()->body, cpv(level->spawn_point.x, level->spawn_point.y));
            cpBodySetVelocity(OBJECTS.back()->body, cpvzero);
            cpBodySetAngularVelocity(OBJECTS.back()->body, 0);
            cpBodySetAngle(OBJECTS.back()->body, 0);
            main_menu->close();
            level_selector->close();
            editor->cleanup();
            PAUSE = false;
        }, nullptr);
            level_count++;
        }
    }, nullptr);
    for(Game::Level* level : game->get_levels() )
    {
        level_count ++;
        float x = 315;
        float y = 225 + 25 * level_count;
        level_selector->add_button(std::to_string(level_count), GetFontDefault(), {x, y}, 100, 10, [level, game, main_menu, level_selector, editor](Menu*, void* data)
        {
            game->start_level(level);
            cpBodySetPosition(OBJECTS.back()->body, cpv(level->spawn_point.x, level->spawn_point.y));
            cpBodySetVelocity(OBJECTS.back()->body, cpvzero);
            cpBodySetAngularVelocity(OBJECTS.back()->body, 0);
            cpBodySetAngle(OBJECTS.back()->body, 0);
            main_menu->close();
            level_selector->close();
            editor->cleanup();
            PAUSE = false;
        }, nullptr);
    }
    main_menu->darken_background = true;
    main_menu->add_label("DropKing", GetFontDefault(), 64, {100, 32});
    main_menu->add_label("Main Menu", GetFontDefault(), 32, {100, 160});
    main_menu->add_button("Quick Start", GetFontDefault(), {100, 200}, 100, 10, [main_menu, game, level_selector](Menu*, void* data)
    {
        game->start();
        main_menu->close();
        level_selector->close();
        PAUSE = false;
    }, nullptr);
    main_menu->add_button("Select Level", GetFontDefault(), {100, 225}, 100, 10, [level_selector](Menu*, void* data)
    {
        level_selector->open();
    }, nullptr);
    main_menu->add_button("Editor", GetFontDefault(), {100, 250}, 250, 10, [main_menu](Menu*, void* data)
    {
        Editor* editor = static_cast<Editor*>(data);
        editor->start();
        main_menu->close();
    }, editor);
    main_menu->add_button("Quit", GetFontDefault(), {100, 275}, 100, 10, [](Menu*, void*) { SHOULD_CLOSE = true; }, nullptr);
    main_menu->open();
    //Player player({-250, 0}, 100, 100);
    MemoryManager::get_usage();
    while (!WindowShouldClose() && !SHOULD_CLOSE)
    {
        fixed_update_accumulator += GetFrameTime();
        if(IsKeyPressed(KEY_ESCAPE))
        {
            PAUSE = true;
            main_menu->open();
        }
        if (fixed_update_accumulator >= fixed_update_interval && !PAUSE)
        {
            //store.update_fixed();
            //debug_submenu.update_fixed();
            //menu.update_fixed();
            //editor->update_fixed();
            //fixed_update_accumulator -= fixed_update_interval;
        }

        if (fixed_update_accumulator >= fixed_update_interval)
        {
            //store.update_fixed();
            //debug_submenu.update_fixed();
            menu.update_fixed();
            editor->update_fixed();
            main_menu->update_fixed();
            level_selector->update_fixed();
            fixed_update_accumulator -= fixed_update_interval;
        }
        
        main_menu->update();
        level_selector->update();
        OBJECTS.back()->update();
        BeginDrawing();
        ClearBackground(DARKGRAY);
        //store.draw();
        //store.update();
        //debug_submenu.draw();
        //debug_submenu.update();
        if (IsKeyPressed(KEY_F1))
        {
            DEBUG = !DEBUG;
        }
        if (DEBUG)
        {
            //EntityManager::draw_debug(camera);
        }
        if (DEBUG_FOOTER)
        {
            int usage = MemoryManager::get_usage();
            const std::string memory_usage = std::format("Memory Usage: {}/Bytes", usage);
            DrawRectangle(0, GetScreenHeight() - 24, GetScreenWidth(), 24, BLACK);
            DrawText(memory_usage.c_str(), 10, GetScreenHeight() - 16, 10, WHITE);
            const std::string entity_count = std::format("Entity Count: {}", EntityManager::get_entity_count());
            DrawText(entity_count.c_str(), 200, GetScreenHeight() - 16, 10, WHITE);
        }

        frame++;
        
        mouse = GetMousePosition();

        if( !PAUSE )
            cpSpaceStep(Physics::Instances::SPACE, 1.0 / 60.0);

        BeginMode2D(camera);
        for (int n = 0; n < OBJECTS.size(); n++)
        {
            cpVect pos = cpBodyGetPosition(OBJECTS[n]->body);
            float a = cpBodyGetAngle(OBJECTS[n]->body);
                Rectangle rect = {(float)pos.x, (float)pos.y, OBJECTS[n]->size.x, OBJECTS[n]->size.y};
                DrawRectanglePro(rect, {OBJECTS[n]->size.x / 2, OBJECTS[n]->size.y / 2},
                                 cpBodyGetAngle(OBJECTS[n]->body) * RAD2DEG,RED);
                Vector2 p[4] = {0};
                Vector2 sz;
                sz.x = OBJECTS[n]->size.x / 2.0;
                sz.y = OBJECTS[n]->size.y / 2.0;
                p[0] = {-sz.x, -sz.y};
                p[1] = {sz.x, -sz.y};
                p[2] = {sz.x, sz.y};
                p[3] = {-sz.x, sz.y};

                for (int i = 0; i < 4; i++)
                {
                    float x = p[i].x;
                    float y = p[i].y;
                    p[i].x = (cos(a) * x) - (sin(a) * y) + pos.x;
                    p[i].y = (cos(a) * y) + (sin(a) * x) + pos.y;
                }
                DrawLine(p[0].x, p[0].y, p[1].x, p[1].y, BLUE);
                DrawLine(p[1].x, p[1].y, p[2].x, p[2].y, BLUE);
                DrawLine(p[2].x, p[2].y, p[3].x, p[3].y, BLUE);
                DrawLine(p[3].x, p[3].y, p[0].x, p[0].y, BLUE);
        }

        cpVect player_position = cpBodyGetPosition(OBJECTS.back()->body);
        //camera.target.x += (player_position.x - camera.target.x) * 0.01;
        //camera.target.y = player_position.y;

        game->update();
        game->draw();
        EndMode2D();
        const char* score_text = TextFormat("Score: %i", SCORE);
        Vector2 score_offset = MeasureTextEx(GetFontDefault(), score_text, 12, 0);

        cpVect player_velocity = cpBodyGetVelocity(OBJECTS.back()->body);
        if(player_velocity.y > 0)
        {
            SCORE += (player_position.y / 2 * (60/GetTime())) * GetFrameTime();
            DrawText(TextFormat("Multiplier: %f", 60/GetTime() * GetFrameTime()), 10, 80, 20, WHITE);
        }
        else
        {
            SCORE += static_cast<int>(GetFrameTime());
        }

        editor->update();
        editor->draw();
        main_menu->draw();
        level_selector->draw();
        //DrawText(TextFormat("Velocity: %f", player_velocity.y), 10, 60, 20, WHITE);
        //DrawText(score_text, screen_width / 2 - score_offset.x, 64, 20, WHITE);
        //DrawText(TextFormat("%f", GetTime()), 10, 10, 20, WHITE);
        //DrawFPS(20, 20);
        EndDrawing();

        if( IsKeyPressed(KEY_R))
        {
            //game.start();
            if( Game::Level* level = game->get_active_level() )
                cpBodySetPosition(OBJECTS.back()->body, cpv(level->spawn_point.x, level->spawn_point.y));
            else
                cpBodySetPosition(OBJECTS.back()->body, cpv(0, 0));
            cpBodySetVelocity(OBJECTS.back()->body, cpvzero);
            cpBodySetAngularVelocity(OBJECTS.back()->body, 0);
            cpBodySetAngle(OBJECTS.back()->body, 0);
        }
    }

    EntityManager::free();
    CloseWindow(); // De-Initialization
    MemoryManager::get_usage();

    return 0;
}
