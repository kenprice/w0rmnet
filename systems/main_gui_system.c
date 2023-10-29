#include <stdio.h>
#include "../lib/raygui.h"
#include "ui/custom_raygui.h"
#include "glib.h"
#include "main_gui_system.h"
#include "main_gui_system/area_viewer_window.h"
#include "botnet_system.h"
#include "ui/device_info_panel.h"
#include "utils/rendering.h"
#include "../components/component_registry.h"
#include "../world/world_state.h"
#include "../world/world_map.h"

#define MAX_AREA_VIEWER_WINDOWS 3
#define UI_COMPONENT_PADDING 8
#define UI_TOP_NAVBAR_HEIGHT 34
#define UI_BOTTOM_PANEL_HEIGHT 232

#define LEFT_PANEL_MODE_PLAYER_AREA 0
#define LEFT_PANEL_MODE_WORMS 1

typedef struct {
    Rectangle leftPanelRect;
    Rectangle rightPanelRect;

    int activeLeftPanelMode;

    bool dragMode;
    int dragOffsetX;

    Device* selectedDevice;
} MainGuiState;

MainGuiState mainGuiState;
AreaViewerWindowState areaViewerWindowState[MAX_AREA_VIEWER_WINDOWS];

static void update_panels_player_area_mode();
static void render_top_navbar();
static void render_worms_window();

void initialize_main_gui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    mainGuiState.leftPanelRect = (Rectangle){0, UI_TOP_NAVBAR_HEIGHT, screenWidth/2+1, screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_BOTTOM_PANEL_HEIGHT};
    areaViewerWindowState[0] = init_area_viewer_window(worldState.currentArea, mainGuiState.leftPanelRect);
    areaViewerWindowState[0].camera.zoom = 1.0f;

    mainGuiState.rightPanelRect = (Rectangle){screenWidth/2, 34, screenWidth/2+1, screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_BOTTOM_PANEL_HEIGHT};
    areaViewerWindowState[1] = init_area_viewer_window(&worldMap.regions[0].zones[0].areas[1], mainGuiState.rightPanelRect);
    areaViewerWindowState[1].camera.zoom = 1.0f;

    mainGuiState.selectedDevice = NULL;

    init_device_info_panel();
}

void update_main_gui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // -------------------
    // Refresh left and right panel dimensions
    mainGuiState.leftPanelRect = (Rectangle){0, UI_TOP_NAVBAR_HEIGHT, mainGuiState.leftPanelRect.width,
                                             screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_BOTTOM_PANEL_HEIGHT};
    mainGuiState.rightPanelRect = (Rectangle){mainGuiState.leftPanelRect.width, 34,
                                              screenWidth-(mainGuiState.leftPanelRect.width),
                                              screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_BOTTOM_PANEL_HEIGHT};

    // -------------------
    // Update right panel (currently fixed to some area)
    areaViewerWindowState[1].window.windowBounds.x = mainGuiState.rightPanelRect.x;
    areaViewerWindowState[1].window.windowBounds.y = mainGuiState.rightPanelRect.y;
    areaViewerWindowState[1].window.windowBounds.width = mainGuiState.rightPanelRect.width;
    areaViewerWindowState[1].window.windowBounds.height = mainGuiState.rightPanelRect.height;
    update_area_viewer_window(&areaViewerWindowState[1]);

    // -------------------
    // Update left panel
    switch (mainGuiState.activeLeftPanelMode) {
        case LEFT_PANEL_MODE_PLAYER_AREA:
            update_panels_player_area_mode();
            break;
        case LEFT_PANEL_MODE_WORMS:
            break;
    }

    // -------------------
    // Drag to adjust side
    Vector2 mousePosition = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle boundaryRect = (Rectangle){mainGuiState.leftPanelRect.width-3, mainGuiState.leftPanelRect.y, 6, mainGuiState.leftPanelRect.height};
        if (CheckCollisionPointRec(mousePosition, boundaryRect)) {
            mainGuiState.dragMode = true;
            mainGuiState.dragOffsetX = mousePosition.x - mainGuiState.leftPanelRect.width;
        }
    }
    if (mainGuiState.dragMode) {
        mainGuiState.leftPanelRect.width = mousePosition.x - mainGuiState.dragOffsetX;
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        mainGuiState.dragMode = false;
    }
}

void render_main_gui_system() {
    int screenWidth = GetScreenWidth();

    // Left info panel
    Rectangle infoPanelRect = (Rectangle){
        mainGuiState.leftPanelRect.x, mainGuiState.leftPanelRect.y+mainGuiState.leftPanelRect.height,
        screenWidth/2+1, UI_BOTTOM_PANEL_HEIGHT
    };
    if (render_device_info_panel(infoPanelRect, mainGuiState.selectedDevice)) {
        areaViewerWindowState[0].selectedDevice = NULL;
        areaViewerWindowState[1].selectedDevice = NULL;
        mainGuiState.selectedDevice = NULL;
    }

    // Right log panel
    Rectangle logPanelRect = (Rectangle){
        mainGuiState.leftPanelRect.x+(screenWidth/2), mainGuiState.leftPanelRect.y+mainGuiState.leftPanelRect.height,
        screenWidth/2, UI_BOTTOM_PANEL_HEIGHT
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
    render_top_navbar();

    // -------------------
    // Render right panel (currently fixed to some area)
    render_area_viewer_window(&areaViewerWindowState[1]);

    // -------------------
    // Update left panel
    switch (mainGuiState.activeLeftPanelMode) {
        case LEFT_PANEL_MODE_PLAYER_AREA:
            render_area_viewer_window(&areaViewerWindowState[0]);
            break;
        case LEFT_PANEL_MODE_WORMS:
            render_worms_window();
            break;
    }
}

static void update_panels_player_area_mode() {
    areaViewerWindowState[0].window.windowBounds.x = mainGuiState.leftPanelRect.x;
    areaViewerWindowState[0].window.windowBounds.y = mainGuiState.leftPanelRect.y;
    areaViewerWindowState[0].window.windowBounds.width = mainGuiState.leftPanelRect.width;
    areaViewerWindowState[0].window.windowBounds.height = mainGuiState.leftPanelRect.height;

    if (areaViewerWindowState[0].selectedDevice && mainGuiState.selectedDevice != areaViewerWindowState[0].selectedDevice) {
        mainGuiState.selectedDevice = areaViewerWindowState[0].selectedDevice;
        areaViewerWindowState[1].selectedDevice = NULL;
    }
    if (areaViewerWindowState[1].selectedDevice && mainGuiState.selectedDevice != areaViewerWindowState[1].selectedDevice) {
        mainGuiState.selectedDevice = areaViewerWindowState[1].selectedDevice;
        areaViewerWindowState[0].selectedDevice = NULL;
    }

    update_area_viewer_window(&areaViewerWindowState[0]);
}

static void render_top_navbar() {
    int screenWidth = GetScreenWidth();
    Vector2 mousePos = GetMousePosition();

    Rectangle rectangle = (Rectangle){0, 1, screenWidth, 34};
    GuiPanel(rectangle, NULL);

    Rectangle btnRect = (Rectangle){6, 6, 24, 24};
    GuiToggleGroup(btnRect, "#149#;#152#", &mainGuiState.activeLeftPanelMode);

    Rectangle btnGroupRect = (Rectangle){ rectangle.x+btnRect.x, rectangle.y+btnRect.y, btnRect.width, btnRect.height };
    if (CheckCollisionPointRec(mousePos, btnGroupRect)) {
        GuiTooltipCustom(btnGroupRect, "Player Area");
    }
    btnGroupRect.x += 24 + 4;
    if (CheckCollisionPointRec(mousePos, btnGroupRect)) {
        GuiTooltipCustom(btnGroupRect, "Worms");
    }
}

static void render_worms_window() {
    GuiPanel(mainGuiState.leftPanelRect, "w0rms");

    Rectangle btnRect = (Rectangle){mainGuiState.leftPanelRect.x+20, mainGuiState.leftPanelRect.y+20, 24, 24};

    if (GuiButton(btnRect, "XX")) {
        botnet_system_test_launch_login_attack();
    }
}
