#ifndef EXAMPLE_ENTITIES_H
#define EXAMPLE_ENTITIES_H

#include "components/components.h"
#include "components/component_registry.h"

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

char* create_machine_full(ComponentRegistry* registry, Machine machine);

char* create_router_full(ComponentRegistry* registry, Router router);

#endif //EXAMPLE_ENTITIES_H
