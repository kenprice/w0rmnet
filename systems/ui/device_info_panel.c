#include <stdio.h>
#include <string.h>
#include "glib.h"
#include "device_info_panel.h"
#include "../../components/component_registry.h"
#include "../../lib/raygui.h"

#define TITLEBAR_HEIGHT 24
#define UI_COMPONENT_PADDING 8

typedef struct {
    int activePanelIndex;

    int progScrollIndex;
    int progActiveIndex;
} DeviceInfoPanelState;

DeviceInfoPanelState state;

void init_device_info_panel() {
    state.activePanelIndex = 0;
}

void render_device_prog_controls(Rectangle rect, Device* device) {
    ProcessManager* processManager = (ProcessManager *)g_hash_table_lookup(componentRegistry.processManagers, device->entityId);
    if (!processManager) {
        return;
    }

    Rectangle listviewRect = (Rectangle){rect.x, rect.y, 100, rect.height - (UI_COMPONENT_PADDING*2)};

    char buffer[100] = "";
    if (processManager->numProcs > 0) {
        for (int i=0; i<processManager->numProcs; i++) {
            strcat(buffer, ProcessTypeLabel[processManager->processes[i].type]);
            strcat(buffer, ";");
        }
        buffer[strlen(buffer)-1] = 0;
    }

    GuiListView(listviewRect, buffer, &state.progScrollIndex, &state.progActiveIndex);
}

void render_device_info_stats(Rectangle rect, Device* device) {
    char buffer[100];
    int lineHeight = 16;

    Rectangle infoTextRect = (Rectangle){
            rect.x,
            rect.y,
            300,
            lineHeight
    };

    sprintf(buffer, device->owner == DEVICE_OWNER_PLAYER ? "#138#Owner: You" : "#137#Owner: ???");
    GuiLabel(infoTextRect, buffer);
    infoTextRect.y += lineHeight;

    sprintf(buffer, "%sType: %s", device->type == DEVICE_TYPE_ROUTER ? "#225#" : "#224#", DeviceTypeLabel[device->type]);
    GuiLabel(infoTextRect, buffer);
    infoTextRect.y += lineHeight;
}

void GuiTooltipCustom(Rectangle controlRec, char* text)
{
    Vector2 textSize = MeasureTextEx(GuiGetFont(), text, (float)GuiGetStyle(DEFAULT, TEXT_SIZE), (float)GuiGetStyle(DEFAULT, TEXT_SPACING));

    if ((controlRec.x + textSize.x + 16) > GetScreenWidth()) controlRec.x -= (textSize.x + 16 - controlRec.width);

    GuiPanel((Rectangle){ controlRec.x + controlRec.width + 4, controlRec.y, textSize.x + 16, GuiGetStyle(DEFAULT, TEXT_SIZE) + 8.f }, NULL);

    int textPadding = GuiGetStyle(LABEL, TEXT_PADDING);
    int textAlignment = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
    GuiSetStyle(LABEL, TEXT_PADDING, 0);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle){ controlRec.x + controlRec.width + 4, controlRec.y, textSize.x + 16, GuiGetStyle(DEFAULT, TEXT_SIZE) + 8.f }, text);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, textAlignment);
    GuiSetStyle(LABEL, TEXT_PADDING, textPadding);
}

void render_device_info_menu_buttons(Rectangle rect) {
    Vector2 mousePos = GetMousePosition();

    GuiPanel((Rectangle){rect.x, rect.y + TITLEBAR_HEIGHT - 1,24 + (2*UI_COMPONENT_PADDING) + 1, rect.height-TITLEBAR_HEIGHT+1}, NULL);

    Rectangle btnGroupRect = (Rectangle){
        rect.x + UI_COMPONENT_PADDING,
        rect.y + UI_COMPONENT_PADDING + TITLEBAR_HEIGHT,
        24, 24
    };

    if (GuiButton(btnGroupRect, "#240#")) {
        state.activePanelIndex = 0;
    }
    if (CheckCollisionPointRec(mousePos, btnGroupRect))
        GuiTooltipCustom(btnGroupRect, "Information and stats");

    btnGroupRect.y += 30;
    if (GuiButton(btnGroupRect, "#243#")) {
        state.activePanelIndex = 1;
    }
    if (CheckCollisionPointRec(mousePos, btnGroupRect))
        GuiTooltipCustom(btnGroupRect, "Programs");
}

int render_device_info_panel(Rectangle rect, Device* device) {
    char buffer[100];

    if (!device) return 0;

    sprintf(buffer, "%s %s (%s)", (device->type == DEVICE_TYPE_ROUTER ? "#225#" : "#224#"), device->name, device->address);
    int active = GuiWindowBox(rect, buffer);

    Rectangle activePanelRect = (Rectangle){
        rect.x + 24 + (UI_COMPONENT_PADDING*3),
        rect.y + UI_COMPONENT_PADDING + TITLEBAR_HEIGHT,
        rect.width - 24 - (UI_COMPONENT_PADDING*2),
        rect.height - TITLEBAR_HEIGHT
    };

    if (state.activePanelIndex == 0) {
        render_device_info_stats(activePanelRect, device);
    } else if (state.activePanelIndex == 1 ){
        render_device_prog_controls(activePanelRect, device);
    }

    render_device_info_menu_buttons(rect);

    return active;
}

