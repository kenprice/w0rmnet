#include <stdio.h>
#include "../lib/raygui.h"
#include "glib.h"
#include "device_ui_system.h"
#include "ui/area_viewer_window.h"
#include "ui/device_info_window.h"
#include "utils/rendering.h"
#include "../components/component_registry.h"
#include "../world/world_state.h"
#include "../world/world_map.h"

#define MAX_AREA_VIEWER_WINDOWS 3

AreaViewerWindowState areaViewerWindowState[MAX_AREA_VIEWER_WINDOWS];
int numAreaViewerWindows = 0;

void initialize_device_ui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    areaViewerWindowState[0] = init_area_viewer_window(worldState.currentArea);
    areaViewerWindowState[0].window.windowBounds.x = 0;
    areaViewerWindowState[0].window.windowBounds.y = 20;
    areaViewerWindowState[0].window.windowBounds.width = screenWidth/2;
    areaViewerWindowState[0].window.windowBounds.height = 2*screenHeight/3;
    areaViewerWindowState[0].camera.zoom = 1.0f;


    areaViewerWindowState[1] = init_area_viewer_window(&worldMap.regions[0].zones[0].areas[1]);
    areaViewerWindowState[1].window.windowBounds.x = screenWidth/2;
    areaViewerWindowState[1].window.windowBounds.y = 20;
    areaViewerWindowState[1].window.windowBounds.width = screenWidth/2;
    areaViewerWindowState[1].window.windowBounds.height = 2*screenHeight/3;
    areaViewerWindowState[1].camera.zoom = 1.0f;
}

void update_device_ui_system() {
    update_area_viewer_window(&areaViewerWindowState[0]);
    update_area_viewer_window(&areaViewerWindowState[1]);
}

void render_device_ui_system() {
    render_area_viewer_window(&areaViewerWindowState[0]);
    render_area_viewer_window(&areaViewerWindowState[1]);
}

