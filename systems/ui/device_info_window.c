#include <stdio.h>
#include "glib.h"
#include "raylib.h"
#include "device_info_window.h"
#include "../../lib/raygui.h"
#include "../../components/component_registry.h"
#include "../../world/world_map.h"

#define TITLEBAR_HEIGHT 24
#define STATUSBAR_HEIGHT 18
#define UI_COMPONENT_PADDING 8
#define WINDOW_HEIGHT 310
#define WINDOW_WIDTH 440

DeviceInfoWindowState init_device_info_window(Device* device) {
    DeviceInfoWindowState state;

    // Init window data
    state.windowBounds = (Rectangle){
        0,
        (float)GetScreenHeight() - WINDOW_HEIGHT,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    };
    state.windowActive = false;
    state.supportDrag = true;
    state.dragMode = false;
    state.panOffset = (Vector2){ 0, 0 };

    char buffer[50] = "#181#";
    strcat(buffer, (device != NULL ? device->id : "Device"));
    strcpy(state.windowTitle, buffer);

    // Device info data
    state.device = device;
    state.activeToggleGroup = 0;

    // Progs
    state.progScrollIndex = 0;
    state.progActiveIndex = 0;
    state.progTargetDevice[0] = '\0';
    state.progTargetDeviceAddress[0] = '\0';

    return state;
}

int update_device_info_window(DeviceInfoWindowState* state) {
    if (!state->windowActive) return 0;

    Vector2 mousePosition = GetMousePosition();

    if (state->supportDrag) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Window can be dragged from the top window bar
            if (CheckCollisionPointRec(mousePosition, (Rectangle) {state->windowBounds.x, state->windowBounds.y,
                                                                   (float) state->windowBounds.width,
                                                                   TITLEBAR_HEIGHT})) {
                state->dragMode = true;
                state->panOffset.x = mousePosition.x - state->windowBounds.x;
                state->panOffset.y = mousePosition.y - state->windowBounds.y;
            }
        }

        if (state->dragMode) {
            state->windowBounds.x = (mousePosition.x - state->panOffset.x);
            state->windowBounds.y = (mousePosition.y - state->panOffset.y);

            // Check screen limits to avoid moving out of screen
            if (state->windowBounds.x < 0) state->windowBounds.x = 0;
            else if (state->windowBounds.x > (GetScreenWidth() - state->windowBounds.width)) state->windowBounds.x =
                                                                                                     GetScreenWidth() -
                                                                                                     state->windowBounds.width;

            if (state->windowBounds.y < 0) state->windowBounds.y = 0;
            else if (state->windowBounds.y > (GetScreenHeight() - state->windowBounds.height)) state->windowBounds.y =
                                                                                                       GetScreenHeight() -
                                                                                                       state->windowBounds.height;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) state->dragMode = false;
        }
    }

    return CheckCollisionPointRec(mousePosition, state->windowBounds);
}

void render_info_panel(DeviceInfoWindowState* state) {
    Rectangle groupBoxRect = (Rectangle){
            state->windowBounds.x+UI_COMPONENT_PADDING,
            state->windowBounds.y + (TITLEBAR_HEIGHT*2) + (UI_COMPONENT_PADDING*2),
            state->windowBounds.width - (UI_COMPONENT_PADDING*2),
            state->windowBounds.height - (TITLEBAR_HEIGHT*2) - (UI_COMPONENT_PADDING*3) - STATUSBAR_HEIGHT
    };

    GuiGroupBox(groupBoxRect, "Info");

    char buffer[100];
    sprintf(buffer, "#240#ID: %s", state->device->id);
    Rectangle infoTextRect = (Rectangle){groupBoxRect.x+UI_COMPONENT_PADDING, groupBoxRect.y+(UI_COMPONENT_PADDING*2), groupBoxRect.width, 10};
    GuiLabel(infoTextRect, buffer);
    infoTextRect.y += 16;

    sprintf(buffer, state->device->owner == DEVICE_OWNER_PLAYER ? "#138#Owner: You" : "#137#Owner: ???");
    GuiLabel(infoTextRect, buffer);
    infoTextRect.y += 16;

    sprintf(buffer, "%sType: %s", state->device->type == DEVICE_TYPE_ROUTER ? "#225#" : "#224#", DeviceTypeLabel[state->device->type]);
    GuiLabel(infoTextRect, buffer);
    infoTextRect.y += 16;

    char* network = NULL;
    Connection* conn = (Connection*)g_hash_table_lookup(component_registry.connections, state->device->entity_id);
    if (conn != NULL && strlen(conn->parent_device_id) > 0) {
        network = conn->parent_device_id;
    }
    if (network) {
        sprintf(buffer, "#241#Network: %s", network);
        GuiLabel(infoTextRect, buffer);
        infoTextRect.y += 16;
    }

    sprintf(buffer, "#242#%s", state->device->address);
    GuiLabel(infoTextRect, buffer);
    infoTextRect.y += 16;
}

int render_device_target_dropdown(DeviceInfoWindowState* state, Rectangle rect) {
    if (!state->device) return 0;

    GHashTableIter iter;
    char* entityId;
    Device* device;
    g_hash_table_iter_init(&iter, component_registry.devices);
    float offsetY = 0;

    while (g_hash_table_iter_next (&iter, (gpointer) &entityId, (gpointer) &device)) {
        if (!device) continue;
        if (!device->visible) continue;

        char buffer[100];
        if (strcmp(state->progTargetDevice, device->entity_id) == 0) {
            sprintf(buffer, "#119#%s", device->address);
        } else {
            sprintf(buffer, "#0#%s", device->address);
        }
        if (GuiLabelButton((Rectangle){rect.x, rect.y+offsetY, rect.width, rect.height}, buffer)) {
            strcpy(state->progTargetDevice, device->entity_id);
        }
        offsetY += 20;
    }

    Rectangle actionButtonRect = (Rectangle){
            state->windowBounds.x + state->windowBounds.width-100-(UI_COMPONENT_PADDING),
            state->windowBounds.y + state->windowBounds.height-30-(UI_COMPONENT_PADDING*3),
            100, 30
    };

    return GuiButton(actionButtonRect, "Go");
}

void refreshProgTargetDeviceAddress(DeviceInfoWindowState* state) {
    Device* device = (Device*) g_hash_table_lookup(component_registry.devices, state->progTargetDevice);
    if (!device) return;

    char** splitOriginAddress = g_strsplit(state->device->address, ".", 10);
    char** splitTargetAddress = g_strsplit(device->address, ".", 10);
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

    strcpy(state->progTargetDeviceAddress, targetAddress);
}

void render_progs_ping_options(DeviceInfoWindowState* state) {
    Rectangle groupBoxRect = (Rectangle){
            state->windowBounds.x+100+(UI_COMPONENT_PADDING*2),
            state->windowBounds.y + (TITLEBAR_HEIGHT*2) + (UI_COMPONENT_PADDING*2),
            state->windowBounds.width-100-(UI_COMPONENT_PADDING*3), 24
    };

    if (render_device_target_dropdown(state, groupBoxRect)) {
        refreshProgTargetDeviceAddress(state);

        // ACTION: Send PING
        ProcMessage* msg = proc_msg_alloc(state->progActiveIndex, state->progTargetDeviceAddress);
        proc_msg_queue_write(g_hash_table_lookup(component_registry.proc_msg_queues, state->device->entity_id), msg);
    }
}

void render_progs_scan_options(DeviceInfoWindowState* state) {
    Rectangle groupBoxRect = (Rectangle){
            state->windowBounds.x+100+(UI_COMPONENT_PADDING*2),
            state->windowBounds.y + (TITLEBAR_HEIGHT*2) + (UI_COMPONENT_PADDING*2),
            state->windowBounds.width-100-(UI_COMPONENT_PADDING*3), 24
    };

    if (render_device_target_dropdown(state, groupBoxRect)) {
        refreshProgTargetDeviceAddress(state);

        // ACTION: Send SCAN
        ProcMessage* msg = proc_msg_alloc(state->progActiveIndex, state->progTargetDeviceAddress);
        proc_msg_queue_write(g_hash_table_lookup(component_registry.proc_msg_queues, state->device->entity_id), msg);
    }
}

void render_progs_panel(DeviceInfoWindowState* state) {
    ProcessManager* processManager = (ProcessManager *)g_hash_table_lookup(component_registry.process_managers, state->device->entity_id);
    if (!processManager) {
        return;
    }

    Rectangle listviewRect = (Rectangle){
            state->windowBounds.x+UI_COMPONENT_PADDING,
            state->windowBounds.y + (TITLEBAR_HEIGHT*2) + (UI_COMPONENT_PADDING*2),
            100, state->windowBounds.height - (TITLEBAR_HEIGHT*2) - (UI_COMPONENT_PADDING*3) - STATUSBAR_HEIGHT
    };

    char buffer[100] = "";
    if (processManager->num_procs > 0) {
        for (int i=0; i<processManager->num_procs; i++) {
            strcat(buffer, ProcessTypeLabel[processManager->processes[i].type]);
            strcat(buffer, ";");
        }
        buffer[strlen(buffer)-1] = 0;
    }

    GuiListView(listviewRect, buffer, &state->progScrollIndex, &state->progActiveIndex);

    if (state->progActiveIndex < 0 || state->progActiveIndex >= processManager->num_procs || processManager->num_procs == 0) {
        return;
    }

    switch(processManager->processes[state->progActiveIndex].type) {
        case PROCESS_TYPE_PING:
            render_progs_ping_options(state);
            break;
        case PROCESS_TYPE_SCAN:
            render_progs_scan_options(state);
            break;
    }
}

int render_device_window(DeviceInfoWindowState* state) {
    if (!state->windowActive) return 0;

    state->windowActive = !GuiWindowBox(state->windowBounds, state->windowTitle);

    Rectangle toggleGroupRect = (Rectangle){
        state->windowBounds.x+UI_COMPONENT_PADDING,
        state->windowBounds.y + TITLEBAR_HEIGHT + UI_COMPONENT_PADDING,
        (state->windowBounds.width-(UI_COMPONENT_PADDING*2)-3)/3, TITLEBAR_HEIGHT
    };

    GuiToggleGroup(toggleGroupRect, "INFO;LOGS;PROGS", &state->activeToggleGroup);

    if (!state->device) return 0;

    switch(state->activeToggleGroup) {
        case 0:
            render_info_panel(state);
            break;
        case 2:
            render_progs_panel(state);
            break;
    }

    GuiStatusBar((Rectangle){
            state->windowBounds.x, state->windowBounds.y + state->windowBounds.height - STATUSBAR_HEIGHT,
            state->windowBounds.width, STATUSBAR_HEIGHT
    }, "Online");

    return !state->windowActive;
}
