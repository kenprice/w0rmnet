#include "toolwindow.h"
#include "../../lib/raygui.h"

#define TITLEBAR_HEIGHT 24
#define PAD_8 8

void init_tool_window(ToolWindowState* toolWindowState) {
    switch(toolWindowState->activeToolWindow) {
        case TOOLWINDOW_NETWORK_MAP:
            init_network_map_toolwindow(&toolWindowState->networkMapState, toolWindowState->toolWindowRect);
            break;
        case TOOLWINDOW_WORMS:
            init_worms_toolwindow(&toolWindowState->wormsWindowState, toolWindowState->toolWindowRect);
            break;
    }
}

void update_tool_window(ToolWindowState* toolWindowState) {
    switch(toolWindowState->activeToolWindow) {
        case TOOLWINDOW_NETWORK_MAP:
            update_network_map_toolwindow(&toolWindowState->networkMapState, toolWindowState->toolWindowRect);
            break;
        case TOOLWINDOW_WORMS:
            update_worms_toolwindow(&toolWindowState->wormsWindowState, toolWindowState->toolWindowRect);
            break;
    }
}

void render_tool_window(ToolWindowState* toolWindowState) {
    switch(toolWindowState->activeToolWindow) {
        case TOOLWINDOW_NETWORK_MAP:
            if (GuiWindowBox(toolWindowState->toolWindowRect, "Network")) {
                toolWindowState->activeToolWindow = TOOLWINDOW_INACTIVE;
                toolWindowState->isClosing = true;
            }
            render_network_map_toolwindow(&toolWindowState->networkMapState, toolWindowState->toolWindowRect);
            break;
        case TOOLWINDOW_WORMS:
            if (GuiWindowBox(toolWindowState->toolWindowRect, "Worms")) {
                toolWindowState->activeToolWindow = TOOLWINDOW_INACTIVE;
                toolWindowState->isClosing = true;
            }
            render_worms_toolwindow(&toolWindowState->wormsWindowState, toolWindowState->toolWindowRect);
            break;
    }
}
