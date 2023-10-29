#include <string.h>
#include <stdio.h>
#include "glib.h"
#include "network_map_toolwindow.h"
#include "../../../lib/raygui.h"

#define TITLEBAR_HEIGHT 24
#define UI_COMPONENT_PADDING 8

void render_network_map_toolwindow(ToolWindowState* toolWindowState) {
    GuiWindowBox(toolWindowState->toolWindowRect, "Network Map");

    Rectangle scrollPanelRect = toolWindowState->scrollPanelRect;
    int lineHeight = 24;
    int testNumLines = 50;
    int logLinesHeight = testNumLines * 16;

    Rectangle panelContentRect = (Rectangle){ scrollPanelRect.x, scrollPanelRect.y,
                                              scrollPanelRect.width - UI_COMPONENT_PADDING, logLinesHeight };

    GuiScrollPanel(scrollPanelRect, NULL, panelContentRect, &toolWindowState->scrollPanelScroll, &toolWindowState->scrollPanelView);

    Rectangle infoTextRect = (Rectangle){ scrollPanelRect.x + UI_COMPONENT_PADDING,
                                          scrollPanelRect.y + toolWindowState->scrollPanelScroll.y,
                                          panelContentRect.width - UI_COMPONENT_PADDING, 16 };

    BeginScissorMode(toolWindowState->scrollPanelView.x, toolWindowState->scrollPanelView.y,
                     toolWindowState->scrollPanelView.width, toolWindowState->scrollPanelView.height);
    for (int i = 0; i < testNumLines; i++) {
        char buffer[100];
        sprintf(buffer, "Placeholder %d", i);
        GuiLabel(infoTextRect, buffer);
        infoTextRect.y += lineHeight;
    }
    EndScissorMode();
}
