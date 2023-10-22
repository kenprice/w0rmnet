#include <stdio.h>
#include "../lib/raygui.h"
#include "glib.h"
#include "main_gui_system.h"
#include "ui/area_viewer_window.h"
#include "ui/device_info_panel.h"
#include "utils/rendering.h"
#include "../components/component_registry.h"
#include "../world/world_state.h"
#include "../world/world_map.h"

#define MAX_AREA_VIEWER_WINDOWS 3

typedef struct {
    Rectangle leftPanelRect;
    Rectangle rightPanelRect;
} MainGuiState;

MainGuiState mainGuiState;

AreaViewerWindowState areaViewerWindowState[MAX_AREA_VIEWER_WINDOWS];
int numAreaViewerWindows = 0;

void initialize_main_gui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    mainGuiState.leftPanelRect = (Rectangle){0, 34, screenWidth/2+1, 2*screenHeight/3+1};
    areaViewerWindowState[0] = init_area_viewer_window(worldState.currentArea, mainGuiState.leftPanelRect);
    areaViewerWindowState[0].camera.zoom = 1.0f;

    mainGuiState.rightPanelRect = (Rectangle){screenWidth/2, 34, screenWidth/4, (2*screenHeight/3)/2};
    areaViewerWindowState[1] = init_area_viewer_window(&worldMap.regions[0].zones[0].areas[1], mainGuiState.rightPanelRect);
    areaViewerWindowState[1].camera.zoom = 0.5f;

    init_device_info_panel();
}

void update_main_gui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    mainGuiState.leftPanelRect = (Rectangle){0, 34, screenWidth/2+1, 2*screenHeight/3+1};
    areaViewerWindowState[0].window.windowBounds.x = mainGuiState.leftPanelRect.x;
    areaViewerWindowState[0].window.windowBounds.y = mainGuiState.leftPanelRect.y;
    areaViewerWindowState[0].window.windowBounds.width = mainGuiState.leftPanelRect.width;
    areaViewerWindowState[0].window.windowBounds.height = mainGuiState.leftPanelRect.height;

    mainGuiState.rightPanelRect = (Rectangle){screenWidth/2, 34, screenWidth/4, (2*screenHeight/3)/2};
    areaViewerWindowState[1].window.windowBounds.x = mainGuiState.rightPanelRect.x;
    areaViewerWindowState[1].window.windowBounds.y = mainGuiState.rightPanelRect.y;
    areaViewerWindowState[1].window.windowBounds.width = mainGuiState.rightPanelRect.width;
    areaViewerWindowState[1].window.windowBounds.height = mainGuiState.rightPanelRect.height;
    areaViewerWindowState[1].camera.zoom = 0.5f;

    update_area_viewer_window(&areaViewerWindowState[0]);
    update_area_viewer_window(&areaViewerWindowState[1]);
}



static int active = 0;

void render_main_gui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Rectangle leftPanel = mainGuiState.leftPanelRect;
    render_area_viewer_window(&areaViewerWindowState[0]);

    Rectangle infoPanelRect = (Rectangle){
        leftPanel.x, leftPanel.y+(2*screenHeight/3),
        screenWidth/2+1, screenHeight - areaViewerWindowState[0].window.windowBounds.height - 34
    };

    if (render_device_info_panel(infoPanelRect,areaViewerWindowState[0].selectedDevice)) {
        areaViewerWindowState[0].selectedDevice = NULL;
    }

    Rectangle rectangle = (Rectangle){0, 1, screenWidth, 34};
    GuiPanel(rectangle, NULL);

    GuiToggleGroup((Rectangle){6, 6, 24, 24}, "#149#;#149#", &active);
//    GuiButton((Rectangle){12, 9, 24, 24}, "#149#");

    rectangle = (Rectangle){screenWidth/2, 1, screenWidth/2, 34};
//    GuiPanel(rectangle, NULL);

    // Right panel quartered
    Rectangle rightPanelRect = (Rectangle){screenWidth/2, 34, screenWidth/4, (2*screenHeight/3)/2};

    areaViewerWindowState[1].window.windowBounds.x = rightPanelRect.x;
    areaViewerWindowState[1].window.windowBounds.y = rightPanelRect.y;
    areaViewerWindowState[1].window.windowBounds.width = rightPanelRect.width;
    areaViewerWindowState[1].window.windowBounds.height = rightPanelRect.height;
    areaViewerWindowState[1].camera.zoom = 0.5f;
    render_area_viewer_window(&areaViewerWindowState[1]);

//    rightPanelRect = (Rectangle){screenWidth/2, 34, screenWidth/4, (2*screenHeight/3)/2};
//    GuiPanel(rightPanelRect, NULL);
    rightPanelRect = (Rectangle){screenWidth/2+(screenWidth/4), 34+(2*screenHeight/3)/2, screenWidth/4, (2*screenHeight/3)/2};
    GuiPanel(rightPanelRect, NULL);
}
