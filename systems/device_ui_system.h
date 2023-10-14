#ifndef W0RMNET_DEVICE_UI_SYSTEM_H
#define W0RMNET_DEVICE_UI_SYSTEM_H

#include "raylib.h"
#include "../components/components.h"

extern

void initialize_device_ui_system(Texture2D loaded_texture, Shader loaded_shader_outline);

void update_device_ui_system();

void render_device_ui_system();

#endif //W0RMNET_DEVICE_UI_SYSTEM_H
