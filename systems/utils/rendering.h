#ifndef W0RMNET_RENDERING_H
#define W0RMNET_RENDERING_H

#include "raylib.h"

extern Camera2D camera;

void initialize_camera(int screen_width, int screen_height);

void update_camera();

#endif //W0RMNET_RENDERING_H
