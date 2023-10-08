#ifndef EXAMPLE_ENTITIES_H
#define EXAMPLE_ENTITIES_H

#include "components.h"
#include "component_registry.h"

// Entity Archetypes
typedef struct {
    Device device;
    Position position;
    Sprite sprite;
    OperatingSystem os;
    Connection connection;
} Machine;

typedef struct {
    Device device;
    Position position;
    Sprite sprite;
    Connection connection; // TODO: Extend to many connections
} Router;

void create_entities(ComponentRegistry* registry);

void create_machine(ComponentRegistry* registry, char* id, float x, float y);

char* create_machine_full(ComponentRegistry* registry, Machine machine);

void create_router(ComponentRegistry* registry, char* id, float x, float y);

char* create_router_full(ComponentRegistry* registry, Router router);

#endif //EXAMPLE_ENTITIES_H
