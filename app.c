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

#include "graphics/sprites.h"
#include "graphics/tiles.h"
#include "components/component_registry.h"
#include "systems/device_rendering_system.h"
#include "systems/device_ui_system.h"
#include "systems/packet_routing_system.h"
#include "systems/process_running_system.h"
#include "world/world_map.h"

#define GLSL_VERSION 330

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1024;
    const int screenHeight = 768;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "w0rmnet");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    load_sprite_sheet();
    load_tile_sheet();

    // RayGui
    GuiLoadStyleCyber();

    //////////////
    init_component_registry();
    init_player_area();

    initialize_device_rendering_system();
    initialize_device_ui_system();
    initialize_packet_routing_system();
    //////////////

    srand(time(NULL));   // Initialization, should only be called once.

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        update_device_rendering_system();
        update_device_ui_system();
        update_packet_routing_system();
        update_process_running_system();

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        render_device_rendering_system();
        render_device_ui_system();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture_sprite_sheet);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}