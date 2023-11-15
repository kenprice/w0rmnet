#ifndef W0RMNET_WORMS_TOOLWINDOW_H
#define W0RMNET_WORMS_TOOLWINDOW_H

#include "raylib.h"

typedef struct {
    Rectangle scrollPanelRect;
    Rectangle scrollPanelView;
    Vector2 scrollPanelScroll;
} WormsWindowState;

void init_worms_toolwindow(WormsWindowState* state, Rectangle toolWindowRect);

void update_worms_toolwindow(WormsWindowState* state, Rectangle toolWindowRect);

void render_worms_toolwindow(WormsWindowState* state, Rectangle toolWindowRect);

#endif //W0RMNET_WORMS_TOOLWINDOW_H
