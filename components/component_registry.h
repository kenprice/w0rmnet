#ifndef EXAMPLE_COMPONENT_REGISTRY_H
#define EXAMPLE_COMPONENT_REGISTRY_H

#include <glib.h>
#include "raylib.h"
#include "../graphics/sprites.h"

typedef struct {
    GHashTable* devices;
    GHashTable* positions;
    GHashTable* sprites;
    GHashTable* connections;
    GHashTable* packet_buffers;
    GHashTable* route_tables;
} ComponentRegistry;

extern ComponentRegistry component_registry;

void init_component_registry();

#endif //EXAMPLE_COMPONENT_REGISTRY_H
