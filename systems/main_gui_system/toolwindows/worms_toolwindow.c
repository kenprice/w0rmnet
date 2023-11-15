#include <string.h>
#include <stdio.h>
#include "glib.h"
#include "worms_toolwindow.h"
#include "../../../lib/raygui.h"

#define TITLEBAR_HEIGHT 24
#define UI_COMPONENT_PADDING 8

void init_worms_toolwindow(WormsWindowState* state, Rectangle toolWindowRect) {
    state->scrollPanelView = (Rectangle){ 0 };
    state->scrollPanelScroll = (Vector2){ 0 };
    state->scrollPanelRect = (Rectangle){
            toolWindowRect.x,
            toolWindowRect.y + TITLEBAR_HEIGHT-1,
            toolWindowRect.width,
            toolWindowRect.height - TITLEBAR_HEIGHT + 1,
    };
}

void update_worms_toolwindow(WormsWindowState* state, Rectangle toolWindowRect) {
    state->scrollPanelRect = (Rectangle){
            toolWindowRect.x,
            toolWindowRect.y + TITLEBAR_HEIGHT-1,
            toolWindowRect.width,
            toolWindowRect.height - TITLEBAR_HEIGHT + 1,
    };
}

void render_worms_toolwindow(WormsWindowState* state, Rectangle toolWindowRect) {
    Rectangle scrollPanelRect = state->scrollPanelRect;
    int lineHeight = 24;
    int testNumLines = 50;
    int logLinesHeight = testNumLines * lineHeight;

    Rectangle panelContentRect = (Rectangle){ scrollPanelRect.x, scrollPanelRect.y,
                                              scrollPanelRect.width - (UI_COMPONENT_PADDING*2), logLinesHeight };

    GuiScrollPanel(scrollPanelRect, NULL, panelContentRect, &state->scrollPanelScroll, &state->scrollPanelView);

    Rectangle infoTextRect = (Rectangle){ scrollPanelRect.x + UI_COMPONENT_PADDING,
                                          scrollPanelRect.y + state->scrollPanelScroll.y + UI_COMPONENT_PADDING,
                                          panelContentRect.width - UI_COMPONENT_PADDING, 16 };

    char buffer[100];
    BeginScissorMode(state->scrollPanelView.x, state->scrollPanelView.y, state->scrollPanelView.width, state->scrollPanelView.height);
    sprintf(buffer, "New Worm");
    if (GuiLabelButton(infoTextRect, buffer)) {
//        mainGuiState.activeLeftPanelMode = LEFT_PANEL_MODE_WORMS;
    }
    EndScissorMode();
}
