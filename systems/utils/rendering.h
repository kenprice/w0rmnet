#ifndef W0RMNET_RENDERING_H
#define W0RMNET_RENDERING_H

#include "raylib.h"

extern Camera2D camera;

void initialize_camera(int screen_width, int screen_height);

void update_camera();

Vector2 convert_local_to_global(float x, float y);

Vector2 convert_global_to_local(float x, float y);

void draw_mouse_coords();

#endif //W0RMNET_RENDERING_H
