#ifndef EXAMPLE_COMPONENT_REGISTRY_H
#define EXAMPLE_COMPONENT_REGISTRY_H

#include <glib.h>
#include "raylib.h"
#include "../graphics/sprites.h"

typedef struct {
    GHashTable* devices;
    GHashTable* positions;
    GHashTable* sprites;
    GHashTable* packetBuffers;
    GHashTable* routeTable;
    GHashTable* processManagers;
    GHashTable* procMsgQueues;
    GHashTable* wires;
    GHashTable* polygons;
    GHashTable* logger;
    GHashTable* loots;
    GHashTable* infections;
    GHashTable* knownHosts;
} ComponentRegistry;

extern ComponentRegistry componentRegistry;

void init_component_registry();

#endif //EXAMPLE_COMPONENT_REGISTRY_H
