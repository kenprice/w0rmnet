#ifndef EXAMPLE_COMPONENT_REGISTRY_H
#define EXAMPLE_COMPONENT_REGISTRY_H

#include <glib.h>
#include "raylib.h"
#include "components.h"
#include "sprites.h"

typedef struct {
    GHashTable* devices;
    GHashTable* positions;
    GHashTable* sprites;
    GHashTable* connections;
} ComponentRegistry;

void init_component_registry(ComponentRegistry* registry);

// System
void device_rendering_system(Texture2D texture, ComponentRegistry* registry);

#endif //EXAMPLE_COMPONENT_REGISTRY_H
