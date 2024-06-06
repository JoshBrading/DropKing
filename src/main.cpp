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

bool DEBUG = false;
bool DEBUG_FOOTER = false;
bool SHOULD_CLOSE = false;

int SCORE = 0;

int main(void)
{
    // Init window
    //SetConfigFlags(FLAG_VSYNC_HINT); 
    constexpr int screen_width = 1920;
    constexpr int screen_height = 1000;
    InitWindow(screen_width, screen_height, "DropKing - [raylib]");
    SetExitKey(KEY_NULL);

    Camera2D camera = {0};
    camera.offset = {screen_width / 2, screen_height / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    SetTargetFPS(60);

    constexpr float fixed_update_interval = 1.0f / 60.0f;
    float fixed_update_accumulator = 0.0f;

    Game::GameWorld* game = new Game::GameWorld(&camera);
    Editor* editor = new Editor(&camera, game);
    
    int frame = 0;

    Vector2 mouse = {0.0f, 0.0f};
    Physics::init();
    cpSpaceSetGravity(Physics::Instances::SPACE, Physics::GRAVITY);
    
    game->load_levels();

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
            main_menu->close();
            level_selector->close();
            editor->cleanup();
            Game::PAUSE = true;
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
            main_menu->close();
            level_selector->close();
            editor->cleanup();
            Game::PAUSE = true;
        }, nullptr);
    }
    main_menu->darken_background = true;
    main_menu->add_label("DropKing", GetFontDefault(), 64, {100, 32});
    main_menu->add_label("Programming & Design by Josh Brading", GetFontDefault(), 20, {screen_width - 450.0f, screen_height - 50.0f});
    main_menu->add_label("Art by Daniel Giust", GetFontDefault(), 20, {screen_width - 250.0f, screen_height - 100.0f});
    main_menu->add_label("Main Menu", GetFontDefault(), 32, {100, 160});
    main_menu->add_button("Quick Start", GetFontDefault(), {100, 200}, 100, 10, [main_menu, game, level_selector](Menu*, void* data)
    {
        game->start();
        main_menu->close();
        level_selector->close();
        Game::PAUSE = false;
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

    MemoryManager::get_usage();
    while (!WindowShouldClose() && !SHOULD_CLOSE)
    {
        fixed_update_accumulator += GetFrameTime();
        if(IsKeyPressed(KEY_ESCAPE))
        {
            Game::PAUSE = true;
            main_menu->open();
        }

        if (fixed_update_accumulator >= fixed_update_interval)
        {
            editor->update_fixed();
            main_menu->update_fixed();
            level_selector->update_fixed();
            game->update_fixed();
            fixed_update_accumulator -= fixed_update_interval;
        }
        
        main_menu->update();
        level_selector->update();
        BeginDrawing();
        ClearBackground({32, 32, 32, 255});
        if (IsKeyPressed(KEY_F1))
        {
            DEBUG = !DEBUG;
        }
        if (DEBUG)
        {
            int usage = MemoryManager::get_usage();
            const std::string memory_usage = std::format("Memory Usage: {}/Bytes", usage);
            DrawRectangle(0, GetScreenHeight() - 24, GetScreenWidth(), 24, BLACK);
            DrawText(memory_usage.c_str(), 10, GetScreenHeight() - 16, 10, WHITE);
        }

        frame++;
        
        mouse = GetMousePosition();

        if( !Game::PAUSE )
            cpSpaceStep(Physics::Instances::SPACE, 1.0 / 60.0);

        BeginMode2D(camera);
        game->draw();
        EndMode2D();
        game->update();
        const char* score_text = TextFormat("Score: %i", SCORE);
        Vector2 score_offset = MeasureTextEx(GetFontDefault(), score_text, 12, 0);
        editor->update();
        editor->draw();
        main_menu->draw();
        level_selector->draw();
        EndDrawing();
    }
    CloseWindow();
    MemoryManager::get_usage();

    return 0;
}
