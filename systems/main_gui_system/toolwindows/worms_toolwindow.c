#include <string.h>
#include <stdio.h>
#include "glib.h"
#include "worms_toolwindow.h"
#include "../../../lib/raygui.h"
#include "../worms_window.h"

#define TITLEBAR_HEIGHT 24
#define PAD_8 8

void init_worms_toolwindow(WormsToolWindowState* state, Rectangle toolWindowRect) {
    state->scrollPanelView = (Rectangle){ 0 };
    state->scrollPanelScroll = (Vector2){ 0 };
    state->scrollPanelRect = (Rectangle){
            toolWindowRect.x,
            toolWindowRect.y + TITLEBAR_HEIGHT-1,
            toolWindowRect.width,
            toolWindowRect.height - TITLEBAR_HEIGHT + 1,
    };
}

void update_worms_toolwindow(WormsToolWindowState* state, Rectangle toolWindowRect) {
    state->scrollPanelRect = (Rectangle){
            toolWindowRect.x,
            toolWindowRect.y + TITLEBAR_HEIGHT-1,
            toolWindowRect.width,
            toolWindowRect.height - TITLEBAR_HEIGHT + 1,
    };
}

void render_worms_toolwindow(WormsToolWindowState* state, Rectangle toolWindowRect) {
    Rectangle scrollPanelRect = state->scrollPanelRect;
    int lineHeight = 24;
    int testNumLines = 50;
    int logLinesHeight = testNumLines * lineHeight;

    Rectangle panelContentRect = (Rectangle){scrollPanelRect.x, scrollPanelRect.y,
                                              scrollPanelRect.width - (PAD_8 * 2), logLinesHeight };

    GuiScrollPanel(scrollPanelRect, NULL, panelContentRect, &state->scrollPanelScroll, &state->scrollPanelView);

    Rectangle newItemRect = (Rectangle){scrollPanelRect.x + PAD_8,
                                        scrollPanelRect.y + state->scrollPanelScroll.y + PAD_8,
                                        panelContentRect.width+2-PAD_8*2, lineHeight};
    Rectangle lineRect = newItemRect;
    lineRect.y += lineHeight - PAD_8/2;

    char buffer[100];
    sprintf(buffer, "#008# New Worm");

    BeginScissorMode(state->scrollPanelView.x, state->scrollPanelView.y, state->scrollPanelView.width, state->scrollPanelView.height);
    if (GuiLabelButton(newItemRect, buffer)) {
        init_worms_window();
    }
    GuiLine(lineRect, NULL);
    EndScissorMode();
}
