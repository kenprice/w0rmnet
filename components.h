//
// Created by Ken Price on 10/7/23.
//

#ifndef EXAMPLE_COMPONENTS_H
#define EXAMPLE_COMPONENTS_H
#include <string.h>
#include "raylib.h"

// Components
typedef struct {
    char id[32];
} Device;

typedef struct {
    int x, y;
} Position;

typedef struct {
    char* img_path;
    int w, h;
    int a_x, a_y; // "Anchor" to isometric tile
} Tile;

typedef struct {
    char id[32];
} OperatingSystem;

typedef struct {
    Device devices[100];
    int devices_total;
    Position positions[100];
    int positions_total;
    Tile tiles[100];
    int tiles_total;
} ComponentLists;


// Entities
typedef struct {
    int entity_id;
    Device device;
    Position position;
    Tile tile;
    OperatingSystem os;
} Machine;

typedef struct {
    int entity_id;
    Device device;
    Position position;
    Tile tile;
} Router;

int NUM_ENTITIES = 0;

void create_entities(ComponentLists* components) {
    // Create some Machine
    strcpy(components->devices[NUM_ENTITIES].id, "machine1337");
    components->positions[NUM_ENTITIES].x = 3;
    components->positions[NUM_ENTITIES].y = 3;
    components->tiles[NUM_ENTITIES].img_path = "../resources/server-128.png";
    NUM_ENTITIES++;

    // Create some Router
    strcpy(components->devices[NUM_ENTITIES].id, "router777");
    components->positions[NUM_ENTITIES].x = 6;
    components->positions[NUM_ENTITIES].y = 3;
    components->tiles[NUM_ENTITIES].img_path = "../resources/load-balancer-128.png";
    NUM_ENTITIES++;
}


// System
void device_rendering_system(ComponentLists* components) {
    for (int i = 0; i < NUM_ENTITIES; i++) {
        Image image = LoadImage(components->tiles[i].img_path);  // Load image data into CPU memory (RAM)
        Texture2D texture = LoadTextureFromImage(image);       // Image converted to texture, GPU memory (RAM -> VRAM)
        UnloadImage(image);                                    // Unload image data from CPU memory (RAM)

        image = LoadImageFromTexture(texture);                 // Load image from GPU texture (VRAM -> RAM)
        UnloadTexture(texture);                                // Unload texture from GPU memory (VRAM)

        texture = LoadTextureFromImage(image);                 // Recreate texture from retrieved image data (RAM -> VRAM)
        UnloadImage(image);                                    // Unload retrieved image data from CPU memory (RAM)

        DrawTexture(texture, components->positions[i].x * 32, components->positions[i].y * 32, WHITE);

//        UnloadTexture(texture);       // Texture unloading
    }
}

#endif //EXAMPLE_COMPONENTS_H
