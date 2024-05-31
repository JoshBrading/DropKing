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

#include <chipmunk/chipmunk.h>

#include "player.h"

enum shapeType
{
    s_circle = 0,
    s_box = 1
};

typedef struct phys
{
    enum shapeType type;
    Vector2 size;
    cpBody* body;
    cpShape* shape;
    bool sharedBody;
} phys;

#define MaxObj 40
phys Obj[MaxObj];
cpSpace* space;

cpConstraint* makePivot(int o, cpVect pos)
{
    Obj[o].type = s_box;
    Obj[o].size = {96, 8};
    Obj[o].body = cpSpaceAddBody(space, cpBodyNew(0, 0));
    Obj[o].sharedBody = false;
    Obj[o].shape = cpSpaceAddShape(space, cpBoxShapeNew(Obj[o].body, Obj[o].size.x, Obj[o].size.y, 0.0));
    cpShapeSetFriction(Obj[o].shape, 0.6);
    cpShapeSetMass(Obj[o].shape, 8);

    Obj[o + 1].type = s_box;
    Obj[o + 1].size = {8, 96};
    Obj[o + 1].body = Obj[o].body;
    Obj[o + 1].sharedBody = true;
    Obj[o + 1].shape = cpSpaceAddShape(space, cpBoxShapeNew(Obj[o].body, Obj[o + 1].size.x, Obj[o + 1].size.y, 0.0));
    cpShapeSetFriction(Obj[o + 1].shape, 0.6);
    cpShapeSetMass(Obj[o + 1].shape, 8);

    cpBodySetPosition(Obj[o].body, pos);
    return cpSpaceAddConstraint(space, cpPivotJointNew(Obj[o].body, cpSpaceGetStaticBody(space), pos));
}

void postSolve(cpArbiter* arb, cpSpace* space, cpDataPointer data)
{
    (void)space;
    (void)data;
    if (cpArbiterIsFirstContact(arb))
    {
        //TraceLog(LOG_INFO,"total KE %f", cpArbiterTotalKE(arb));
        float f = cpArbiterTotalKE(arb);
        if (f > 20000)
        {
            f = 20000;
        }
    }
}

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

    Camera2D camera = {0};
    camera.target = {screen_width / 2.0, screen_height / 2.0};
    camera.offset = {0, 0};
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
    debug_submenu.add_button("Toggle Pause Menu", GetFontDefault(), {GetScreenWidth() - 415.0f, 150}, 100, 10,
                             [&menu](Menu*, void*)
                             {
                                 PAUSE = !PAUSE;
                                 menu.toggle();
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
    store.toggle();
    store.darken_background = false;


    // frame counter
    int frame = 0;

    // for mouse dragging
    Vector2 lastMouse = {0.0f, 0.0f};
    Vector2 mouse = {0.0f, 0.0f};
    bool dragging = false;

    cpVect gravity = cpv(0, 98);
    space = cpSpaceNew();
    cpSpaceSetGravity(space, gravity);

    cpCollisionHandler* handler = cpSpaceAddCollisionHandler(space, 0, 0);
    handler->postSolveFunc = (cpCollisionPostSolveFunc)postSolve;

    cpShape* slope = cpSegmentShapeNew(cpSpaceGetStaticBody(space), cpv(120, 400), cpv(1160, 800), 0);
    cpShapeSetFriction(slope, .8);
    cpSpaceAddShape(space, slope);


    for (int n = 6; n < MaxObj; n++)
    {
        cpFloat mass;
        if (n % 2)
        {
            Obj[n].type = s_box;
            Obj[n].size.x = GetRandomValue(24, 48);
            Obj[n].size.y = GetRandomValue(24, 48);
            mass = (Obj[n].size.x * Obj[n].size.y) * 0.001;
            Obj[n].body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForBox(mass, Obj[n].size.x, Obj[n].size.y)));
            Obj[n].sharedBody = false;
            Obj[n].shape = cpSpaceAddShape(space, cpBoxShapeNew(Obj[n].body, Obj[n].size.x, Obj[n].size.y, 0.0));
            cpShapeSetFriction(Obj[n].shape, 0.4);
        }
        else
        {
            Obj[n].type = s_circle;
            Obj[n].size.x = GetRandomValue(12, 24);
            mass = (PI * (Obj[n].size.x * Obj[n].size.x)) * 0.001;
            Obj[n].body = cpSpaceAddBody(space, cpBodyNew(mass, cpMomentForCircle(mass, 0, Obj[n].size.x, cpvzero)));
            Obj[n].sharedBody = false;
            Obj[n].shape = cpSpaceAddShape(space, cpCircleShapeNew(Obj[n].body, Obj[n].size.x, cpvzero));
            cpShapeSetFriction(Obj[n].shape, 0.7);
        }
        cpBodySetPosition(Obj[n].body, cpv(GetRandomValue(120, 1160), GetRandomValue(400, 500)));
    }

    cpConstraint* joints[3];

    joints[0] = makePivot(0, {300, 300});
    joints[1] = makePivot(2, {450, 300});
    joints[2] = makePivot(4, {600, 300});


    //Player player({300, 0}, 100, 100);
    MemoryManager::get_usage();
    while (!SHOULD_CLOSE)
    {
        fixed_update_accumulator += GetFrameTime();

        if (fixed_update_accumulator >= fixed_update_interval && !PAUSE)
        {
            store.update_fixed();
            debug_submenu.update_fixed();
            menu.update_fixed();
            fixed_update_accumulator -= fixed_update_interval;
            
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);
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

        cpSpaceStep(space, 1.0 / 60.0);

        // fake joint friction
        const float drag = 0.999;
        cpBodySetAngularVelocity(Obj[0].body, cpBodyGetAngularVelocity(Obj[0].body) * drag);
        cpBodySetAngularVelocity(Obj[2].body, cpBodyGetAngularVelocity(Obj[2].body) * drag);
        cpBodySetAngularVelocity(Obj[4].body, cpBodyGetAngularVelocity(Obj[4].body) * drag);


        lastMouse = mouse;
        mouse = GetMousePosition();

        float mz = ((float)GetMouseWheelMove()) / 10.0;
        if (mz != 0.0)
        {
            camera.zoom += mz;
            if (camera.zoom < 0.1)
            {
                camera.zoom = 0.1;
            }
        }


        if (dragging)
        {
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) dragging = false;
            float dx = ((float)(mouse.x - lastMouse.x));
            float dy = ((float)(mouse.y - lastMouse.y));
            camera.offset.x += dx;
            camera.offset.y += dy;
            camera.target = camera.offset;
        }
        else
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                dragging = true;
            }
        }

        BeginMode2D(camera);
        // TODO should get shape offset...
        for (int n = 0; n < MaxObj; n++)
        {
            cpVect pos = cpBodyGetPosition(Obj[n].body);
            float a = cpBodyGetAngle(Obj[n].body);
            if (Obj[n].type == s_circle)
            {
                cpFloat r = cpCircleShapeGetRadius(Obj[n].shape);
                DrawCircle(pos.x, pos.y, r, WHITE);
                DrawLine(pos.x, pos.y,
                         pos.x + cos(a) * Obj[n].size.x, pos.y + sin(a) * Obj[n].size.x,BLUE);
            }
            else
            {
                DrawRectanglePro({(float)pos.x, (float)pos.y, Obj[n].size.x, Obj[n].size.y},
                                 {Obj[n].size.x / 2, Obj[n].size.y / 2},
                                 cpBodyGetAngle(Obj[n].body) * RAD2DEG,RED);
                Vector2 p[4] = {0};
                Vector2 sz;
                sz.x = Obj[n].size.x / 2.0;
                sz.y = Obj[n].size.y / 2.0;
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


            if (pos.y > screen_height + 200)
            {
                cpBodySetPosition(Obj[n].body, cpv(GetRandomValue(220, 520), GetRandomValue(100, 250)));
                cpBodySetVelocity(Obj[n].body, cpvzero);
            }
        }

        DrawLine(120, 400, 1160, 800, WHITE);
        EndMode2D();


        DrawFPS(20, 20);
        EndDrawing();
    }

    EntityManager::free();
    CloseWindow(); // De-Initialization
    MemoryManager::get_usage();

    return 0;
}
