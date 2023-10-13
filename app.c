#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "graphics/sprites.h"
#include "entities/entities.h"
#include "components/component_registry.h"
#include "systems/device_rendering_system.h"
#include "systems/packet_routing_system.h"
#include "systems/process_running_system.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1080;
    const int screenHeight = 800;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "w0rmnet");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    Texture2D texture = load_sprite_sheet();


    //////////////
    init_component_registry();
    create_entities();
    initialize_device_rendering_system(texture);
    initialize_packet_routing_system();
    //////////////

    srand(time(NULL));   // Initialization, should only be called once.

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        update_device_rendering_system();
        update_packet_routing_system();
        update_process_running_system();

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(GRAY);

        render_device_rendering_system();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}