#include <string.h>
#include <stdio.h>
#include "glib.h"
#include "toolwindow.h"
#include "toolwindows/network_map_toolwindow.h"
#include "../../lib/raygui.h"

#define TITLEBAR_HEIGHT 24
#define UI_COMPONENT_PADDING 8

void init_tool_window(ToolWindowState* toolWindowState) {
    toolWindowState->scrollPanelView = (Rectangle){ 0 };
    toolWindowState->scrollPanelScroll = (Vector2){ 0 };
    toolWindowState->scrollPanelRect = (Rectangle){
        toolWindowState->toolWindowRect.x,
        toolWindowState->toolWindowRect.y + TITLEBAR_HEIGHT-1,
        toolWindowState->toolWindowRect.width,
        toolWindowState->toolWindowRect.height - TITLEBAR_HEIGHT + 1,
    };
}

void update_tool_window(ToolWindowState* toolWindowState) {
    switch(toolWindowState->activeToolWindow) {
        case TOOLWINDOW_NETWORK_MAP:
            strcpy(toolWindowState->toolWindowTitle, "#241# Network Map");
            break;
    }
}

void render_tool_window(ToolWindowState* toolWindowState) {
    if (GuiWindowBox(toolWindowState->toolWindowRect, toolWindowState->toolWindowTitle)) {
        toolWindowState->activeToolWindow = -1;
    }

    switch(toolWindowState->activeToolWindow) {
        case TOOLWINDOW_NETWORK_MAP:
            render_network_map_toolwindow(toolWindowState);
            break;
    }
}
