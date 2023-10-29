#ifndef W0RMNET_TOOLWINDOW_H
#define W0RMNET_TOOLWINDOW_H

#include "raylib.h"

#define UI_LEFT_TOOLWINDOW_WIDTH 240
#define TOOLWINDOW_NETWORK_MAP 2

typedef struct {
    Rectangle toolWindowRect;
    int activeToolWindow;

    char toolWindowTitle[200];

    Rectangle scrollPanelRect;
    Rectangle scrollPanelView;
    Vector2 scrollPanelScroll;
} ToolWindowState;

void init_tool_window(ToolWindowState* toolWindowState);

void update_tool_window(ToolWindowState* toolWindowState);

void render_tool_window(ToolWindowState* toolWindowState);

#endif //W0RMNET_TOOLWINDOW_H
