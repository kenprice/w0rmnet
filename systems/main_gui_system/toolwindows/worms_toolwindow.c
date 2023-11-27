#include <string.h>
#include <stdio.h>
#include "glib.h"
#include "worms_toolwindow.h"
#include "../worms_window.h"
#include "../../main_gui_system.h"
#include "../../ui/custom_raygui.h"
#include "../../../events/worm_events.h"
#include "../../../lib/log/log.h"
#include "../../../lib/raygui.h"
#include "../../../lib/text_rectangle_bounds.h"
#include "../../../world/world_state.h"
#include "../../../systems/worm_system.h"

#define TITLEBAR_HEIGHT 24
#define PAD_8 8

bool worms_toolwindow_render_worm_item(WormsToolWindowState* state, int x, int y, int width, Worm* worm, bool active);

void init_worms_toolwindow(WormsToolWindowState* state, Rectangle toolWindowRect) {
    state->scrollPanelView = (Rectangle){ 0 };
    state->scrollPanelScroll = (Vector2){ 0 };
    state->scrollPanelRect = (Rectangle){
            toolWindowRect.x,
            toolWindowRect.y + TITLEBAR_HEIGHT-1,
            toolWindowRect.width,
            toolWindowRect.height - TITLEBAR_HEIGHT + 1,
    };
    state->selectedWormIdx = -1;
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

    Rectangle panelContentRect = { scrollPanelRect.x, scrollPanelRect.y,
                                   scrollPanelRect.width - (PAD_8 * 2), logLinesHeight };

    GuiScrollPanel(scrollPanelRect, NULL, panelContentRect, &state->scrollPanelScroll, &state->scrollPanelView);

    Rectangle newItemRect = { scrollPanelRect.x + PAD_8, scrollPanelRect.y + state->scrollPanelScroll.y + PAD_8,
                              panelContentRect.width+2-PAD_8*2, lineHeight };
    Rectangle lineRect = newItemRect;
    lineRect.y += lineHeight - PAD_8/2;

    int x = newItemRect.x, y = lineRect.y + lineHeight;
    char buffer[100];
    sprintf(buffer, "#008# New Worm");

    BeginScissorMode(state->scrollPanelView.x, state->scrollPanelView.y, state->scrollPanelView.width, state->scrollPanelView.height);
    if (GuiLabelButton(newItemRect, buffer)) {
        init_worms_window();
    }
    GuiLine(lineRect, NULL);
    for (int i = 0; i < worldState.numWorms; i++) {
        if (worms_toolwindow_render_worm_item(state, x, y, scrollPanelRect.width, worldState.worms[i], state->selectedWormIdx == i)) {
            state->selectedWormIdx = i;
        }
        y += 100;
    }
    EndScissorMode();
}

bool worms_toolwindow_render_worm_item(WormsToolWindowState* state, int x, int y, int width, Worm* worm, bool active) {
    Color labelColor = GetColor(GuiGetStyle(LABEL, TEXT_COLOR_NORMAL));
    Color labelHighlightColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));

    DrawRectangle(x, y, width-4-PAD_8*3, 92, ColorAlpha(BLACK, 0.2f));
    GuiDrawIcon(208, x, y, 3, active ? labelHighlightColor : labelColor);

    if (active) GuiSetState(STATE_PRESSED);
    Rectangle labelRect = { x+48, y, width, 14 };
    GuiLabel(labelRect, worm->wormName);
    labelRect.y += 14;

    Device* device = gui_get_selected_device();
    if (active) {
        GuiSetState(STATE_NORMAL);
        char buffer[100] = "Select on map";
        if (device) {
            sprintf(buffer, "Deploy target: %s", device->name);
        }
        GuiLabel(labelRect, buffer);
        GuiSetState(STATE_PRESSED);
    } else {
        char buffer[100] = "";
        worms_copy_worm_summary(worm, buffer);
        labelRect.height = 35; labelRect.width -= 72;
        DrawTextBoxed(GuiGetFont(), buffer, labelRect, 14, 0, true, labelColor);
    }

    Rectangle targetButton = { x+PAD_8, y+56, 80, 24 };
    if (active && device) {
        GuiSetState(STATE_NORMAL);
        if (GuiButton(targetButton, "#113#Cancel ")) {
            state->selectedWormIdx = -1;
        }
        if (device->owner != DEVICE_OWNER_PLAYER) GuiSetState(STATE_DISABLED);
        targetButton.x += 88; targetButton.width = 64;
        if (GuiButton(targetButton, "#152#Send ")) {
            // Launch worm from here
            log_debug("Launching worm from UI");
            events_publish_worm_infected_device_event(worm, gui_get_selected_device());
            state->selectedWormIdx = -1;
        }
        GuiSetState(STATE_PRESSED);
    } else {
        if (GuiButton(targetButton, "#209#Deploy ")) {
            if (active) {
                state->selectedWormIdx = -1;
                gui_set_active_worm(NULL);
                GuiSetState(STATE_NORMAL);
                return false;
            } else {
                gui_set_active_worm(&worm);
            }
            GuiSetState(STATE_NORMAL);
            return true;
        }
    }
    GuiSetState(STATE_NORMAL);
    return false;
}
