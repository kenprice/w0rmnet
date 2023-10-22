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
    char progTargetDevice[UUID_STR_LEN];
    char progTargetDeviceAddress[100];
    char progInputText[100];
    bool progInputTextEditMode;
} DeviceInfoPanelState;

DeviceInfoPanelState state;

void init_device_info_panel() {
    state.activePanelIndex = 0;
}

static int render_device_target_dropdown(Rectangle rect) {
    GHashTableIter iter;
    char* entityId;
    Device* device;
    g_hash_table_iter_init(&iter, componentRegistry.devices);
    float offsetY = 0;

    Rectangle groupBoxRect = (Rectangle){
            rect.x+100+UI_COMPONENT_PADDING, rect.y,
            rect.width-100-(UI_COMPONENT_PADDING*3), 24
    };

    while (g_hash_table_iter_next (&iter, (gpointer) &entityId, (gpointer) &device)) {
        if (!device) continue;
        if (!device->visible) continue;

        char buffer[100];
        if (strcmp(state.progTargetDevice, device->entityId) == 0) {
            sprintf(buffer, "#119#%s", device->address);
        } else {
            sprintf(buffer, "#0#%s", device->address);
        }
        if (GuiLabelButton((Rectangle){groupBoxRect.x, groupBoxRect.y+offsetY, groupBoxRect.width, groupBoxRect.height}, buffer)) {
            strcpy(state.progTargetDevice, device->entityId);
        }
        offsetY += 20;
    }

    Rectangle actionButtonRect = (Rectangle){
            rect.x + rect.width-100-(UI_COMPONENT_PADDING*2),
            rect.y + rect.height-24-(UI_COMPONENT_PADDING*2),
            100, 24
    };

    return GuiButton(actionButtonRect, "Go");
}

/**
 * Resolve target address (for whichever prog uses it) to its simplest form
 *
 * @param state
 */
static void refresh_prog_target_device_address(Device* device) {
    Device* targetDevice = (Device*) g_hash_table_lookup(componentRegistry.devices, state.progTargetDevice);
    if (!targetDevice) return;

    char** splitOriginAddress = g_strsplit(device->address, ".", 10);
    char** splitTargetAddress = g_strsplit(targetDevice->address, ".", 10);
    char targetAddress[100] = "";

    int i;
    for (i = 0; i < 10; i++) {
        if (splitOriginAddress[i] == NULL || splitTargetAddress[i] == NULL) {
            break;
        }
        if (strcmp(splitOriginAddress[i], splitTargetAddress[i]) != 0) {
            break;
        }
    }
    for (int j = i-1; j < 10; j++) {
        if (splitTargetAddress[j] != NULL && strlen(splitTargetAddress[j])) {
            strcat(targetAddress, splitTargetAddress[j]);
            strcat(targetAddress, ".");
        } else {
            break;
        }
    }
    targetAddress[strlen(targetAddress)-1] = '\0';

    strcpy(state.progTargetDeviceAddress, targetAddress);
}

static void render_prog_options_single_target_only(Rectangle rect, Device* device) {
    if (render_device_target_dropdown(rect)) {
        refresh_prog_target_device_address(device);

        // ACTION: Send PING, SCAN, etc.
        ProcMessage* msg = proc_msg_alloc(state.progActiveIndex, state.progTargetDeviceAddress);
        proc_msg_queue_write(g_hash_table_lookup(componentRegistry.procMsgQueues, device->entityId), msg);
    }
}

static void render_progs_login_options(Rectangle rect, Device* device) {
    Rectangle textboxRect = (Rectangle){
        rect.x+100+UI_COMPONENT_PADDING,
        rect.y + rect.height-24-(UI_COMPONENT_PADDING*2),
        rect.width-200-(UI_COMPONENT_PADDING*2), 24
    };

    if (GuiTextBox(textboxRect, state.progInputText, 100, state.progInputTextEditMode)) {
        state.progInputTextEditMode = !state.progInputTextEditMode;
    }

    if (render_device_target_dropdown(rect)) {
        refresh_prog_target_device_address(device);

        // ACTION: Send LOGIN
        char buffer[100];
        strcpy(buffer, state.progTargetDeviceAddress);
        strcat(buffer, ":");
        strcat(buffer, state.progInputText);

        ProcMessage* msg = proc_msg_alloc(state.progActiveIndex, buffer);
        proc_msg_queue_write(g_hash_table_lookup(componentRegistry.procMsgQueues, device->entityId), msg);
    }
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

    if (state.progActiveIndex < 0 || state.progActiveIndex >= processManager->numProcs || processManager->numProcs == 0) {
        return;
    }

    switch(processManager->processes[state.progActiveIndex].type) {
        case PROCESS_TYPE_PING:
            render_prog_options_single_target_only(rect, device);
            break;
        case PROCESS_TYPE_SCAN:
            render_prog_options_single_target_only(rect, device);
            break;
        case PROCESS_TYPE_LOGIN:
            render_progs_login_options(rect, device);
            break;
    }
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

