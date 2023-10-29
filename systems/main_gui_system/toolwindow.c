#include <string.h>
#include "toolwindow.h"
#include "../../lib/raygui.h"

#define TITLEBAR_HEIGHT 24
#define UI_COMPONENT_PADDING 8

void init_tool_window(ToolWindowState* toolWindowState) {
    switch(toolWindowState->activeToolWindow) {
        case TOOLWINDOW_NETWORK_MAP:
            init_network_map_toolwindow(&toolWindowState->networkMapState, toolWindowState->toolWindowRect);
            toolWindowState->networkMapState.switchAreaFn = toolWindowState->switchAreaFn;
            break;
    }
}

void update_tool_window(ToolWindowState* toolWindowState) {
    switch(toolWindowState->activeToolWindow) {
        case TOOLWINDOW_NETWORK_MAP:
            update_network_map_toolwindow(&toolWindowState->networkMapState, toolWindowState->toolWindowRect);
            break;
    }
}

void render_tool_window(ToolWindowState* toolWindowState) {
    switch(toolWindowState->activeToolWindow) {
        case TOOLWINDOW_NETWORK_MAP:
            if (GuiWindowBox(toolWindowState->toolWindowRect, "#242# Network")) {
                toolWindowState->activeToolWindow = -1;
            }
            render_network_map_toolwindow(&toolWindowState->networkMapState, toolWindowState->toolWindowRect);
            break;
    }
}
