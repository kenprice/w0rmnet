#ifndef W0RMNET_POSITION_H
#define W0RMNET_POSITION_H

#include "raylib.h"

typedef struct {
    Vector2 coord;
} Position;

char* comp_position_serialize(Position* position);

Position* comp_position_deserialize(char* data);

#endif //W0RMNET_POSITION_H
