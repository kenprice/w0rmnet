#ifndef W0RMNET_WINDOW_STRUCT_H
#define W0RMNET_WINDOW_STRUCT_H

#include "raylib.h"

// Window management variables
typedef struct {
    bool windowActive;
    Rectangle windowBounds;
    Vector2 panOffset;
    bool dragMode;
    bool supportDrag;
    char windowTitle[100];
} WindowStruct;

#endif //W0RMNET_WINDOW_STRUCT_H
