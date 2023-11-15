#include <string.h>
#include <stdio.h>
#include "glib.h"
#include "network_map_toolwindow.h"
#include "../../../components/component_registry.h"
#include "../../../lib/raygui.h"
#include "../../../world/world_map.h"

#define TITLEBAR_HEIGHT 24
#define UI_COMPONENT_PADDING 8

void init_network_map_toolwindow(NetworkMapWindowState* state, Rectangle toolWindowRect) {
    state->scrollPanelView = (Rectangle){ 0 };
    state->scrollPanelScroll = (Vector2){ 0 };
    state->scrollPanelRect = (Rectangle){
        toolWindowRect.x,
        toolWindowRect.y + TITLEBAR_HEIGHT-1,
        toolWindowRect.width,
        toolWindowRect.height - TITLEBAR_HEIGHT + 1,
    };
}

void update_network_map_toolwindow(NetworkMapWindowState* state, Rectangle toolWindowRect) {
    state->scrollPanelRect = (Rectangle){
        toolWindowRect.x,
        toolWindowRect.y + TITLEBAR_HEIGHT-1,
        toolWindowRect.width,
        toolWindowRect.height - TITLEBAR_HEIGHT + 1,
    };
}

void render_network_map_toolwindow(NetworkMapWindowState* state, Rectangle toolWindowRect) {
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
    for (int r = 0; r < worldMap.numRegions; r++) {
        Region* region = &worldMap.regions[r];
        sprintf(buffer, "#244#%s", region->regionId);
        GuiLabel(infoTextRect, buffer);
        infoTextRect.y += lineHeight;
        infoTextRect.x += lineHeight/4;

        for (int z = 0; z < region->numZones; z++) {
            Zone* zone = &region->zones[z];
            sprintf(buffer, "#245#%s", zone->zoneId);
            GuiLabel(infoTextRect, buffer);
            infoTextRect.y += lineHeight;
            infoTextRect.x += lineHeight/4;

            for (int a = 0; a < zone->numAreas; a++) {
                Area* area = &zone->areas[a];
                sprintf(buffer, "#241#%s", area->areaName);

                if (state->selectedArea == area) GuiSetState(STATE_PRESSED);
                if (GuiLabelButton(infoTextRect, buffer)) {
                    state->switchAreaFn(area);
                    state->selectedArea = area;
                }
                GuiSetState(STATE_NORMAL);
                infoTextRect.y += lineHeight;

                if (state->selectedArea != area) continue;
                infoTextRect.x += lineHeight/4;
                for (int i = 0; i < area->numEntities; i++) {
                    Device* device = g_hash_table_lookup(componentRegistry.devices, area->entities[i]);
                    if (!device || !device->visible) continue;

                    sprintf(buffer, "%s%s", device->type == DEVICE_TYPE_ROUTER ? "#225#" : "#224#", device->name);
                    GuiLabelButton(infoTextRect, buffer);
                    infoTextRect.y += lineHeight;
                }
                infoTextRect.x -= lineHeight/4;
            }
            infoTextRect.x -= lineHeight/4;
        }
        infoTextRect.x -= lineHeight/4;
    }

    EndScissorMode();
}
