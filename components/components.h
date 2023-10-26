#ifndef EXAMPLE_COMPONENTS_H
#define EXAMPLE_COMPONENTS_H

#include "raylib.h"
#include "device.h"
#include "packet.h"
#include "route_table.h"
#include "sprite.h"
#include "process.h"
#include "wire.h"
#include "polygon.h"

// Components

typedef struct {
    Vector2 coord;
} Position;

char* comp_position_serialize(Position* position);

Position* comp_position_deserialize(char* data);

#endif //EXAMPLE_COMPONENTS_H
