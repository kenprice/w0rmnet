#ifndef W0RMNET_DEVICE_INFO_WINDOW_H
#define W0RMNET_DEVICE_INFO_WINDOW_H

#include "raylib.h"
#include "../../components/components.h"

typedef struct {
    // Window management variables
    bool windowActive;
    Rectangle windowBounds;
    Vector2 panOffset;
    bool dragMode;
    bool supportDrag;
    char windowTitle[50];

    // Device info variables
    Device* device;
    Rectangle infoPanelBounds;
    int activeToggleGroup;

    // Progs
    int progScrollIndex;
    int progActiveIndex;

    char progTargetDevice[UUID_STR_LEN]; // entityId
    char progTargetDeviceAddress[110];
} DeviceInfoWindowState;

DeviceInfoWindowState init_device_info_window(Device* device);

int update_device_info_window(DeviceInfoWindowState* state);

/**
 * Returns 1 when transitioning from active to closed
 */
int render_device_window(DeviceInfoWindowState* state);

#endif //W0RMNET_DEVICE_INFO_WINDOW_H
