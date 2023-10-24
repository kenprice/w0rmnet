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
#define UI_COMPONENT_PADDING 8

typedef struct {
    Rectangle leftPanelRect;
    Rectangle rightPanelRect;

    Device* selectedDevice;
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

    mainGuiState.rightPanelRect = (Rectangle){screenWidth/2, 34, screenWidth/2+1, 2*screenHeight/3+1};
    areaViewerWindowState[1] = init_area_viewer_window(&worldMap.regions[0].zones[0].areas[1], mainGuiState.rightPanelRect);
    areaViewerWindowState[1].camera.zoom = 1.0f;

    mainGuiState.selectedDevice = NULL;
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

    mainGuiState.rightPanelRect = (Rectangle){screenWidth/2, 34, screenWidth/2+1, 2*screenHeight/3+1};
    areaViewerWindowState[1].window.windowBounds.x = mainGuiState.rightPanelRect.x;
    areaViewerWindowState[1].window.windowBounds.y = mainGuiState.rightPanelRect.y;
    areaViewerWindowState[1].window.windowBounds.width = mainGuiState.rightPanelRect.width;
    areaViewerWindowState[1].window.windowBounds.height = mainGuiState.rightPanelRect.height;

    update_area_viewer_window(&areaViewerWindowState[0]);
    update_area_viewer_window(&areaViewerWindowState[1]);

    // Janky way of selecting exactly one selected device out of the two area windows
    if (!mainGuiState.selectedDevice) {
        if (areaViewerWindowState[0].selectedDevice) {
            mainGuiState.selectedDevice = areaViewerWindowState[0].selectedDevice;
            areaViewerWindowState[1].selectedDevice = NULL;
        } else if (areaViewerWindowState[1].selectedDevice) {
            mainGuiState.selectedDevice = areaViewerWindowState[1].selectedDevice;
            areaViewerWindowState[0].selectedDevice = NULL;
        }
    } else {
        if (areaViewerWindowState[0].selectedDevice && mainGuiState.selectedDevice != areaViewerWindowState[0].selectedDevice) {
            mainGuiState.selectedDevice = areaViewerWindowState[0].selectedDevice;
            areaViewerWindowState[1].selectedDevice = NULL;
        } else if (areaViewerWindowState[1].selectedDevice && mainGuiState.selectedDevice != areaViewerWindowState[1].selectedDevice) {
            mainGuiState.selectedDevice = areaViewerWindowState[1].selectedDevice;
            areaViewerWindowState[0].selectedDevice = NULL;
        } else if (!areaViewerWindowState[0].selectedDevice && !areaViewerWindowState[1].selectedDevice) {
            mainGuiState.selectedDevice = NULL;
        }
    }
}



static int active = 0;

void render_main_gui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Rectangle leftPanel = mainGuiState.leftPanelRect;

    // Left info panel
    Rectangle infoPanelRect = (Rectangle){
        leftPanel.x, leftPanel.y+(2*screenHeight/3),
        screenWidth/2+1, screenHeight - areaViewerWindowState[0].window.windowBounds.height - 34
    };
    if (render_device_info_panel(infoPanelRect, mainGuiState.selectedDevice)) {
        areaViewerWindowState[0].selectedDevice = NULL;
        areaViewerWindowState[1].selectedDevice = NULL;
        mainGuiState.selectedDevice = NULL;
    }

    // Right log panel
    Rectangle logPanelRect = (Rectangle){
        leftPanel.x+(screenWidth/2), leftPanel.y+(2*screenHeight/3),
        screenWidth/2, screenHeight - areaViewerWindowState[0].window.windowBounds.height - 34
    };
    GuiPanel(logPanelRect, "Logs");

    Rectangle logPanelText = (Rectangle){
        logPanelRect.x + UI_COMPONENT_PADDING,
        logPanelRect.y + TITLEBAR_HEIGHT - 1,
        logPanelRect.width, 24
    };
    char text[1000] = "";
    GuiLabel(logPanelText, text);


    // Top navbar
    Rectangle rectangle = (Rectangle){0, 1, screenWidth, 34};
    GuiPanel(rectangle, NULL);
    GuiToggleGroup((Rectangle){6, 6, 24, 24}, "#149#;#149#", &active);

    // left and right area views
    render_area_viewer_window(&areaViewerWindowState[0]);
    render_area_viewer_window(&areaViewerWindowState[1]);
}