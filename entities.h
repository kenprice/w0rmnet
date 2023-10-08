#ifndef EXAMPLE_ENTITIES_H
#define EXAMPLE_ENTITIES_H

#include "components.h"
#include "component_registry.h"

// Entities
typedef struct {
    Device device;
    Position position;
    Sprite sprite;
    OperatingSystem os;
} Machine;

typedef struct {
    Device device;
    Position position;
    Sprite sprite;
} Router;

void create_entities(ComponentRegistry* registry);

void create_machine(ComponentRegistry* registry, char* id, float x, float y);

void create_router(ComponentRegistry* registry, char* id, float x, float y);

#endif //EXAMPLE_ENTITIES_H
