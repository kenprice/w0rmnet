#ifndef EXAMPLE_COMPONENTS_H
#define EXAMPLE_COMPONENTS_H

#include "raylib.h"
#include "connection.h"
#include "device.h"
#include "packet.h"
#include "route_table.h"
#include "sprite.h"
#include "process.h"

// Components

typedef struct {
    Vector2 coord;
} Position;

// Operating System Components

typedef struct {
    char id[32];
} OperatingSystem;

#endif //EXAMPLE_COMPONENTS_H
