#ifndef W0RMNET_UI_WINDOW_H
#define W0RMNET_UI_WINDOW_H

#include "raylib.h"

#define TITLEBAR_HEIGHT 24

// Window management variables
typedef struct {
    bool windowActive;
    Rectangle windowBounds;
    Vector2 panOffset;
    bool dragMode;
    bool supportDrag;
    char windowTitle[100];
} WindowStruct;

int update_window(WindowStruct* window);

#endif //W0RMNET_UI_WINDOW_H
