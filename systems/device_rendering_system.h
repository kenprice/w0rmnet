#ifndef EXAMPLE_RENDERING_SYSTEM_H
#define EXAMPLE_RENDERING_SYSTEM_H

#include "../component_registry.h"
#include "raylib.h"
#include <glib.h>
#include <stdio.h>

extern Camera2D camera;

void initialize_device_rendering_system();

void update_device_rendering_system();

void render_device_rendering_system(Texture2D texture, ComponentRegistry* registry);

#endif //EXAMPLE_RENDERING_SYSTEM_H
