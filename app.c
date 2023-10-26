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
#include "world/world_map.h"
#include "world/world_state.h"

#define GLSL_VERSION 330

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
//    const int screenWidth = 1920;
//    const int screenHeight = 1080;
    const int screenWidth = 1440;
    const int screenHeight = 847;
    const int minScreenWidth = 1024;
    const int minScreenHeight = 768;
    log_debug("Starting w0rmnet...");

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "w0rmnet");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    load_sprite_sheet();
    load_tile_sheet();

    // RayGui
    GuiLoadStyleCyber();
    GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

    //////////////
    init_component_registry();
    initialize_world();
    init_world_state();

    initialize_main_gui_system();
    initialize_packet_routing_system();
    //////////////

    srand(time(NULL));   // Initialization, should only be called once.

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Window resizing, set minimum
        int curScreenWidth = GetScreenWidth();
        int curScreenHeight = GetScreenHeight();
        if (curScreenWidth < minScreenWidth || curScreenHeight < minScreenHeight) {
            SetWindowSize(curScreenWidth >= minScreenWidth ? curScreenWidth : minScreenWidth,
                          curScreenHeight >= minScreenHeight ? curScreenHeight : minScreenHeight);
        }

        // Update
        update_main_gui_system();
        update_packet_routing_system();
        update_process_running_system();

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        render_main_gui_system();

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