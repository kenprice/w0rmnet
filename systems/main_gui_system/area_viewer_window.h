#ifndef W0RMNET_AREA_VIEWER_WINDOW_H
#define W0RMNET_AREA_VIEWER_WINDOW_H

#include "raylib.h"
#include "../ui/ui_window.h"
#include "../../world/world_map.h"

typedef struct {
    // Window management variables
    WindowStruct window;

    // Area
    Area* area;
    Rectangle viewport;

    // Selected device
    Device* selectedDevice;
    void (*selectDeviceFn)(Device*);

    // Area rendering
    Camera2D camera;
} AreaViewerWindowState;

AreaViewerWindowState init_area_viewer_window(Area* area, Rectangle rect);

int update_area_viewer_window(AreaViewerWindowState* state);

/**
 * Returns 1 when closing
 */
int render_area_viewer_window(AreaViewerWindowState* state);

void area_viewer_center_at_position(AreaViewerWindowState* state, Position* position);

#endif //W0RMNET_AREA_VIEWER_WINDOW_H
