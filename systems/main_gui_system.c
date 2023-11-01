#include <stdio.h>
#include "../lib/raygui.h"
#include "ui/custom_raygui.h"
#include "glib.h"
#include "main_gui_system.h"
#include "main_gui_system/area_viewer_window.h"
#include "main_gui_system/minimap.h"
#include "main_gui_system/toolwindow.h"
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
#define UI_STATUS_BAR_HEIGHT 24

#define UI_LEFT_SIDEBAR_WIDTH 36
#define UI_RIGHT_SIDEBAR_WIDTH 280

#define LEFT_PANEL_MODE_PLAYER_AREA 0
#define LEFT_PANEL_MODE_WORMS 1

typedef struct {
    Rectangle leftPanelRect;
    Rectangle rightPanelRect;

    int activeLeftPanelMode;

    bool dragMode;
    int dragOffsetX;

    Device* selectedDevice;
    bool deviceDrawerActive;

    ToolWindowState toolWindowState;
} MainGuiState;

MainGuiState mainGuiState;
AreaViewerWindowState areaViewerWindowState[MAX_AREA_VIEWER_WINDOWS];

static void update_panels_player_area_mode();
static void render_status_bar();
static void render_device_info_drawer();
static void render_left_navbar();
static void render_top_navbar();
static void render_worms_window();
static void load_area_view_left_panel(Area* area);
static void load_device_info_panel(Device* device);

void initialize_main_gui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    mainGuiState.leftPanelRect = (Rectangle){
        UI_LEFT_SIDEBAR_WIDTH-1,
        UI_TOP_NAVBAR_HEIGHT,
        screenWidth - UI_LEFT_SIDEBAR_WIDTH - UI_RIGHT_SIDEBAR_WIDTH + 1,
        screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_STATUS_BAR_HEIGHT
    };
    areaViewerWindowState[0] = init_area_viewer_window(worldState.currentArea, mainGuiState.leftPanelRect);
    areaViewerWindowState[0].camera.zoom = 1.0f;
    areaViewerWindowState[0].selectDeviceFn = load_device_info_panel;

    mainGuiState.rightPanelRect = (Rectangle){
        mainGuiState.leftPanelRect.x + mainGuiState.leftPanelRect.width - 1,
        UI_TOP_NAVBAR_HEIGHT,
        UI_RIGHT_SIDEBAR_WIDTH,
        screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_STATUS_BAR_HEIGHT
    };
    areaViewerWindowState[1] = init_area_viewer_window(&worldMap.regions[0].zones[0].areas[1], mainGuiState.rightPanelRect);
    areaViewerWindowState[1].camera.zoom = 0.5f;
    areaViewerWindowState[1].selectDeviceFn = load_device_info_panel;

    mainGuiState.selectedDevice = NULL;
    mainGuiState.toolWindowState.activeToolWindow = TOOLWINDOW_INACTIVE;
    mainGuiState.toolWindowState.switchAreaFn = load_area_view_left_panel;
    mainGuiState.toolWindowState.selectDeviceFn = load_device_info_panel;

    init_minimap_view(mainGuiState.rightPanelRect);
    init_device_info_panel();
}

void update_main_gui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // -------------------
    // Refresh left and right panel dimensions

    int toolWindowWidth = mainGuiState.toolWindowState.activeToolWindow == TOOLWINDOW_INACTIVE ? 0 : UI_LEFT_TOOLWINDOW_WIDTH;
    mainGuiState.leftPanelRect = (Rectangle){
        toolWindowWidth + UI_LEFT_SIDEBAR_WIDTH - 1,
        UI_TOP_NAVBAR_HEIGHT,
        screenWidth - UI_LEFT_SIDEBAR_WIDTH - UI_RIGHT_SIDEBAR_WIDTH - toolWindowWidth,
        screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_STATUS_BAR_HEIGHT
    };
    mainGuiState.rightPanelRect = (Rectangle){
        mainGuiState.leftPanelRect.x + mainGuiState.leftPanelRect.width - 1,
        UI_TOP_NAVBAR_HEIGHT,
        UI_RIGHT_SIDEBAR_WIDTH,
        screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_STATUS_BAR_HEIGHT
    };

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
    // Update tool window
    Rectangle toolWindowRect = (Rectangle){
        mainGuiState.leftPanelRect.x - UI_LEFT_TOOLWINDOW_WIDTH,
        UI_TOP_NAVBAR_HEIGHT,
        UI_LEFT_TOOLWINDOW_WIDTH,
        screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_STATUS_BAR_HEIGHT
    };
    mainGuiState.toolWindowState.toolWindowRect = toolWindowRect;
    update_tool_window(&mainGuiState.toolWindowState);
    update_minimap_view(mainGuiState.rightPanelRect);

    // -------------------
    // Drag to adjust side
    Vector2 mousePosition = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle boundaryRect = (Rectangle){mainGuiState.leftPanelRect.x+mainGuiState.leftPanelRect.width-3, mainGuiState.leftPanelRect.y, 6, mainGuiState.leftPanelRect.height};
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

    // -------------------
    // Render right panel (currently fixed to some area)
    render_area_viewer_window(&areaViewerWindowState[1]);

    // -------------------
    // Render left panel
    switch (mainGuiState.activeLeftPanelMode) {
        case LEFT_PANEL_MODE_PLAYER_AREA:
            render_area_viewer_window(&areaViewerWindowState[0]);
            break;
        case LEFT_PANEL_MODE_WORMS:
            render_worms_window();
            break;
    }

    // render minimap
    render_minimap_view(mainGuiState.rightPanelRect);

    // -------------------
    // Top navbar
    render_top_navbar();

    // -------------------
    // Left drawer icons
    render_tool_window(&mainGuiState.toolWindowState);
    render_left_navbar();

    // -------------------
    // Status bar
    render_status_bar();
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

static void render_status_bar() {
    int screenWidth = GetScreenWidth();

    Rectangle statusBarRect = (Rectangle){
        0, mainGuiState.leftPanelRect.y+mainGuiState.leftPanelRect.height,
        screenWidth, UI_STATUS_BAR_HEIGHT
    };

    GuiStatusBar(statusBarRect, NULL);

    Rectangle logBtnRect = statusBarRect;
    logBtnRect.x = UI_COMPONENT_PADDING;
    logBtnRect.width = 100;
    if (mainGuiState.selectedDevice) {
        char buffer[100];
        sprintf(buffer, "%s %s", (mainGuiState.selectedDevice->type == DEVICE_TYPE_ROUTER ? "#225#" : "#224#"), mainGuiState.selectedDevice->address);
        if (mainGuiState.deviceDrawerActive) GuiSetState(STATE_PRESSED);
        if (GuiLabelButton(logBtnRect, buffer)) {
            mainGuiState.deviceDrawerActive = !mainGuiState.deviceDrawerActive;
        }
        GuiSetState(STATE_NORMAL);
        render_device_info_drawer();
    }

    logBtnRect = statusBarRect;
    logBtnRect.x = statusBarRect.width - 50 - UI_COMPONENT_PADDING;
    logBtnRect.width = 50;
    GuiLabelButton(logBtnRect, "#177#LOG");
}

static void render_device_info_drawer() {
    const int deviceInfoDrawerHeight = 400;
    const int deviceInfoDrawerWidth = 500;
    int screenHeight = GetScreenHeight();

    if (!mainGuiState.deviceDrawerActive) return;

    Rectangle drawerRect = (Rectangle){
        0, screenHeight - deviceInfoDrawerHeight - UI_STATUS_BAR_HEIGHT,
        deviceInfoDrawerWidth, deviceInfoDrawerHeight
    };

    GuiUnlock();
    if (render_device_info_panel(drawerRect, mainGuiState.selectedDevice)) {
        mainGuiState.deviceDrawerActive = false;
        return;
    }

    if (CheckCollisionPointRec(GetMousePosition(), drawerRect)) {
        GuiLock();
    } else {
        GuiUnlock();
    }
}

static void render_left_navbar() {
    int screenHeight = GetScreenHeight();
    Vector2 mousePos = GetMousePosition();

    Rectangle rectangle = (Rectangle){
        0, UI_TOP_NAVBAR_HEIGHT,
        UI_LEFT_SIDEBAR_WIDTH, screenHeight-UI_TOP_NAVBAR_HEIGHT-UI_STATUS_BAR_HEIGHT
    };
    GuiPanel(rectangle, NULL);

    Rectangle btnRect = (Rectangle){6, 40, 24, 24};

    if (GuiButton(btnRect, "#241#")) {
        mainGuiState.toolWindowState.activeToolWindow = TOOLWINDOW_NETWORK_MAP;
        init_tool_window(&mainGuiState.toolWindowState);
    }
    if (CheckCollisionPointRec(mousePos, btnRect)) {
        GuiTooltipCustom(btnRect, "Network Map");
    }
    btnRect.y += 30;
    GuiToggle(btnRect, "#246#", NULL);
    if (CheckCollisionPointRec(mousePos, btnRect)) {
        GuiTooltipCustom(btnRect, "Command & Control");
    }
}

static void render_top_navbar() {
    int screenWidth = GetScreenWidth();
    Vector2 mousePos = GetMousePosition();

    Rectangle rectangle = (Rectangle){0, 1, screenWidth, 34};
    GuiPanel(rectangle, NULL);

    Rectangle btnRect = (Rectangle){screenWidth-64, 6, 24, 24};
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

static void load_area_view_left_panel(Area* area) {
    areaViewerWindowState[0] = init_area_viewer_window(area, mainGuiState.leftPanelRect);
    areaViewerWindowState[0].selectDeviceFn = load_device_info_panel;
}

static void load_device_info_panel(Device* device) {
    if (mainGuiState.selectedDevice == device) {
        mainGuiState.deviceDrawerActive = true;
    }
    mainGuiState.selectedDevice = device;
}
