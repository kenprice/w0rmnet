/**
 * Utilities for rendering isometric maps, and converting between global and tile coordinates
 */

#ifndef W0RMNET_ISOMETRIC_MAP_RENDERING_H
#define W0RMNET_ISOMETRIC_MAP_RENDERING_H

#include "raylib.h"

void isometric_map_draw_grid(Rectangle rect, int numXTiles, int numYTiles, float zoom);

void isometric_map_render_tiles(Rectangle rect, int numXTiles, int numYTiles, float zoom);

Vector2 isometric_map_local_to_global(Rectangle rect, float x, float y, float zoom);

Vector2 isometric_map_global_to_local(Rectangle rect, Vector2 offset, float x, float y, float zoom);

#endif //W0RMNET_ISOMETRIC_MAP_RENDERING_H
