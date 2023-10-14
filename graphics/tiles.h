#ifndef W0RMNET_TILES_H
#define W0RMNET_TILES_H

#include "raylib.h"

#define MAP_TILE_HEIGHT 32
#define MAP_TILE_WIDTH 64

typedef struct {
    Rectangle rect; // Section of the spritesheet
    Vector2 offset; // offset by which to render, in order to align to isometric grid
} TileRect;

extern TileRect tile_sheet[];

extern Texture2D texture_tile_sheet;

Texture2D load_tile_sheet();

#endif //W0RMNET_TILES_H
