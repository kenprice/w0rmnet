#ifndef W0RMNET_NETWORK_MAP_TOOLWINDOW_H
#define W0RMNET_NETWORK_MAP_TOOLWINDOW_H

#include "raylib.h"
#include "../../../world/world_map.h"

typedef struct {
    Rectangle scrollPanelRect;
    Rectangle scrollPanelView;
    Vector2 scrollPanelScroll;

    void (*switchAreaFn)(Area*);
} NetworkMapWindowState;

void init_network_map_toolwindow(NetworkMapWindowState* state, Rectangle toolWindowRect);

void update_network_map_toolwindow(NetworkMapWindowState* state, Rectangle toolWindowRect);

void render_network_map_toolwindow(NetworkMapWindowState* state, Rectangle toolWindowRect);

#endif //W0RMNET_NETWORK_MAP_TOOLWINDOW_H
