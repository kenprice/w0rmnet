#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_CUSTOM_ICONS
#include "gui_icons.h"
static unsigned int *guiIconsPtr = guiIcons;
#include "lib/raygui.h"
#include "lib/style_cyber.h"       // raygui style: cyber
#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
#include "lib/log/log.h"

#include "graphics/sprites.h"
#include "graphics/tiles.h"
#include "components/component_registry.h"
#include "systems/main_gui_system.h"
#include "systems/packet_routing_system.h"
#include "systems/process_running_system.h"
#include "systems/botnet_system.h"
#include "systems/world_state_system.h"
#include "systems/worm_system.h"
#include "world/world_map.h"

#define GLSL_VERSION 330

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int initScreenWidth = 1440;
    const int initScreenHeight = 847;
    const int minScreenWidth = 1024;
    const int minScreenHeight = 768;
    int lastScreenWidth = initScreenWidth;
    int lastScreenHeight = initScreenHeight;
    log_debug("Starting w0rmnet...");

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(minScreenWidth, minScreenHeight, "w0rmnet");
    SetWindowSize(initScreenWidth, initScreenHeight);
    SetWindowMinSize(minScreenWidth, minScreenHeight);
    RenderTexture2D renderTexture = LoadRenderTexture(initScreenWidth, initScreenHeight);

    Shader postShader = LoadShader(NULL, TextFormat("resources/shaders/scanlines.fs", GLSL_VERSION));
    int resolutionLoc = GetShaderLocation(postShader, "iResolution");
    int timeLoc = GetShaderLocation(postShader, "iTime");
    float totalTime = 0.0f;
    float resolution[2] = { (float)initScreenWidth, (float)initScreenHeight };
    SetShaderValue(postShader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    load_sprite_sheet();
    load_tile_sheet();

    // RayGui
    GuiLoadStyleCyber();
    GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

    //////////////
    init_component_registry();
    initialize_world();
    init_world_state_system();

    initialize_main_gui_system();
    initialize_packet_routing_system();
    init_worm_system();
    //////////////

    srand(time(NULL));   // Initialization, should only be called once.

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Window resizing, set minimum
        int curScreenWidth = GetScreenWidth();
        int curScreenHeight = GetScreenHeight();
        if (curScreenWidth < minScreenWidth || curScreenHeight < minScreenHeight) {
            curScreenWidth = curScreenWidth >= minScreenWidth ? curScreenWidth : minScreenWidth;
            curScreenHeight = curScreenHeight >= minScreenHeight ? curScreenHeight : minScreenHeight;
            SetWindowSize(curScreenWidth, curScreenHeight);
        }
        if (curScreenWidth != lastScreenWidth || curScreenHeight != lastScreenHeight) {
            lastScreenWidth = curScreenWidth;
            lastScreenHeight = curScreenHeight;

            UnloadRenderTexture(renderTexture);
            renderTexture = LoadRenderTexture(curScreenWidth, curScreenHeight);
            while (!IsRenderTextureReady(renderTexture));

            float resolution[2] = { (float)curScreenWidth, (float)curScreenHeight };
            SetShaderValue(postShader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2);
        }

        // Update
        update_main_gui_system();
        update_process_running_system();
        update_packet_routing_system();
        update_botnet_system();
        update_worm_system();

        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(renderTexture);
            ClearBackground(BLACK);
            render_main_gui_system();
        EndTextureMode();

        totalTime += GetFrameTime();
        SetShaderValue(postShader, timeLoc, &totalTime, SHADER_UNIFORM_FLOAT);

        BeginDrawing();
            BeginShaderMode(postShader);
                DrawTexturePro(renderTexture.texture,
                               (Rectangle){0, 0, curScreenWidth, -curScreenHeight},
                               (Rectangle){0, 0, curScreenWidth, curScreenHeight},
                               (Vector2){ 0 },
                               0.0f,
                               WHITE);
            EndShaderMode();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(textureSpriteSheet);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}