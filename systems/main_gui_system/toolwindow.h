#ifndef W0RMNET_TOOLWINDOW_H
#define W0RMNET_TOOLWINDOW_H

#include "raylib.h"
#include "toolwindows/network_map_toolwindow.h"
#include "../../world/world_map.h"

#define UI_LEFT_TOOLWINDOW_WIDTH 240
#define TOOLWINDOW_INACTIVE -1
#define TOOLWINDOW_NETWORK_MAP 2

typedef struct {
    Rectangle toolWindowRect;
    int activeToolWindow;

    NetworkMapWindowState networkMapState;

    void (*switchAreaFn)(Area*);
} ToolWindowState;

void init_tool_window(ToolWindowState* toolWindowState);

void update_tool_window(ToolWindowState* toolWindowState);

void render_tool_window(ToolWindowState* toolWindowState);

#endif //W0RMNET_TOOLWINDOW_H
