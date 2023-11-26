#include <stdio.h>
#include "glib.h"
#include "../lib/raygui.h"
#include "../world/world_state.h"
#include "ui/custom_raygui.h"
#include "ui/device_info_panel.h"
#include "botnet_system.h"
#include "main_gui_system.h"
#include "main_gui_system/area_viewer_window.h"
#include "main_gui_system/minimap.h"
#include "main_gui_system/recent_events.h"
#include "main_gui_system/toolwindow.h"
#include "main_gui_system/worms_window.h"

#define MAX_AREA_VIEWER_WINDOWS 3
#define PAD_8 8
#define UI_TOP_NAVBAR_HEIGHT 42
#define UI_BOTTOM_PANEL_HEIGHT 232
#define UI_STATUS_BAR_HEIGHT 24

#define UI_LEFT_SIDEBAR_WIDTH 36
#define UI_RIGHT_SIDEBAR_WIDTH 280

#define LEFT_PANEL_MODE_AREA_VIEWER 0

static const int deviceInfoDrawerHeight = 400;
static const int deviceInfoDrawerWidth = 500;

typedef struct {
    Rectangle leftPanelRect;
    Rectangle rightPanelRect;

    int activeLeftPanelMode;

    bool dragMode;
    int dragOffsetX;

    Device* selectedDevice;
    bool deviceDrawerActive;

    ToolWindowState toolWindowState;
    Worm* selectedWorm;
} MainGuiState;

MainGuiState mainGuiState;
AreaViewerWindowState mainAreaViewerWindowState;
AreaViewerWindowState secondaryAreaViewerWindowState;

static void update_panels_player_area_mode();
static void render_status_bar();
static void render_device_info_drawer(Rectangle statusBarRect);
static void render_left_navbar();
static void render_top_navbar();

void initialize_main_gui_system() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    mainGuiState.leftPanelRect = (Rectangle){
        UI_LEFT_SIDEBAR_WIDTH - 1,
        UI_TOP_NAVBAR_HEIGHT,
        screenWidth - UI_LEFT_SIDEBAR_WIDTH - UI_RIGHT_SIDEBAR_WIDTH + 1,
        screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_STATUS_BAR_HEIGHT
    };
    mainAreaViewerWindowState = init_area_viewer_window(worldState.currentArea, mainGuiState.leftPanelRect);
    mainAreaViewerWindowState.camera.zoom = 1.0f;
    mainAreaViewerWindowState.selectDeviceFn = gui_load_device_info_panel;

    mainGuiState.rightPanelRect = (Rectangle){
        mainGuiState.leftPanelRect.x + mainGuiState.leftPanelRect.width + 1,
        UI_TOP_NAVBAR_HEIGHT,
        UI_RIGHT_SIDEBAR_WIDTH - 1,
        UI_RIGHT_SIDEBAR_WIDTH
    };
    secondaryAreaViewerWindowState = init_area_viewer_window(&worldMap.regions[0].zones[0].areas[0], mainGuiState.rightPanelRect);
    secondaryAreaViewerWindowState.camera.zoom = 0.5f;
    secondaryAreaViewerWindowState.selectDeviceFn = NULL;

    mainGuiState.selectedDevice = NULL;
    mainGuiState.toolWindowState.activeToolWindow = TOOLWINDOW_INACTIVE;
    mainGuiState.selectedWorm = NULL;

    init_minimap_view(mainGuiState.rightPanelRect);
    init_device_info_panel();
    init_recent_events_view(&secondaryAreaViewerWindowState);
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
        screenWidth - UI_LEFT_SIDEBAR_WIDTH - UI_RIGHT_SIDEBAR_WIDTH - toolWindowWidth + 1,
        screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_STATUS_BAR_HEIGHT
    };

    // -------------------
    // Update left panel
    switch (mainGuiState.activeLeftPanelMode) {
        case LEFT_PANEL_MODE_AREA_VIEWER:
            update_panels_player_area_mode();
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

    // -------------------
    // Right side minimap, log
    mainGuiState.rightPanelRect = (Rectangle){
            mainGuiState.leftPanelRect.x + mainGuiState.leftPanelRect.width - 1,
            UI_TOP_NAVBAR_HEIGHT,
            UI_RIGHT_SIDEBAR_WIDTH + 1,
            screenHeight - UI_TOP_NAVBAR_HEIGHT - UI_STATUS_BAR_HEIGHT
    };

    update_minimap_view(mainGuiState.rightPanelRect);

    secondaryAreaViewerWindowState.window.windowBounds.x = mainGuiState.rightPanelRect.x;
    secondaryAreaViewerWindowState.window.windowBounds.y = mainGuiState.rightPanelRect.y + UI_MINIMAP_VIEW_HEIGHT;
    secondaryAreaViewerWindowState.window.windowBounds.width = mainGuiState.rightPanelRect.width;
    secondaryAreaViewerWindowState.window.windowBounds.height = UI_MINIMAP_VIEW_HEIGHT;
    update_area_viewer_window(&secondaryAreaViewerWindowState);

    Rectangle eventsRect = mainGuiState.rightPanelRect;
    eventsRect.y += (UI_MINIMAP_VIEW_HEIGHT*2) - 1;
    eventsRect.height = screenHeight - (UI_MINIMAP_VIEW_HEIGHT*2) - UI_TOP_NAVBAR_HEIGHT - UI_STATUS_BAR_HEIGHT + 1;
    update_recent_events_view(eventsRect);

    if (mainGuiState.toolWindowState.isClosing) {
        mainGuiState.toolWindowState.isClosing = false;
        mainAreaViewerWindowState.camera.offset.x += UI_LEFT_TOOLWINDOW_WIDTH * mainAreaViewerWindowState.camera.zoom;
    }
}

void render_main_gui_system() {
    // -------------------
    // Render right panel (currently fixed to some area)
    render_area_viewer_window(&secondaryAreaViewerWindowState);

    // -------------------
    // Render left panel
    switch (mainGuiState.activeLeftPanelMode) {
        case LEFT_PANEL_MODE_AREA_VIEWER:
            render_area_viewer_window(&mainAreaViewerWindowState);
            break;
    }

    // render right panel
    render_minimap_view();
    render_recent_events_view();

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

    render_worms_window();
}

static void update_panels_player_area_mode() {
    mainAreaViewerWindowState.window.windowBounds.x = mainGuiState.leftPanelRect.x;
    mainAreaViewerWindowState.window.windowBounds.y = mainGuiState.leftPanelRect.y;
    mainAreaViewerWindowState.window.windowBounds.width = mainGuiState.leftPanelRect.width;
    mainAreaViewerWindowState.window.windowBounds.height = mainGuiState.leftPanelRect.height;

    if (mainAreaViewerWindowState.selectedDevice && mainGuiState.selectedDevice != mainAreaViewerWindowState.selectedDevice) {
        mainGuiState.selectedDevice = mainAreaViewerWindowState.selectedDevice;
    }

    update_area_viewer_window(&mainAreaViewerWindowState);
}

static void render_status_bar() {
    int screenWidth = GetScreenWidth();

    Rectangle statusBarRect = (Rectangle){
        0, mainGuiState.leftPanelRect.y+mainGuiState.leftPanelRect.height,
        screenWidth, UI_STATUS_BAR_HEIGHT
    };

    GuiStatusBar(statusBarRect, NULL);

    // Selected Device info drawer
    render_device_info_drawer(statusBarRect);

    Rectangle logBtnRect = statusBarRect;
    logBtnRect.x = statusBarRect.width - 50 - PAD_8;
    logBtnRect.width = 50;
    GuiLabelButton(logBtnRect, "#177#LOG");
}

static void render_device_info_drawer(Rectangle statusBarRect) {
    int screenHeight = GetScreenHeight();

    if (mainGuiState.deviceDrawerActive) {
        Rectangle drawerRect = (Rectangle){ 0, screenHeight - deviceInfoDrawerHeight - UI_STATUS_BAR_HEIGHT + 1,
                                            deviceInfoDrawerWidth, deviceInfoDrawerHeight };

        GuiUnlock();
        if (render_device_info_panel(drawerRect, mainGuiState.selectedDevice)) {
            mainGuiState.deviceDrawerActive = false;
            mainGuiState.selectedDevice = NULL;
            mainAreaViewerWindowState.selectedDevice = NULL;
        } else if (CheckCollisionPointRec(GetMousePosition(), drawerRect)) {
            GuiLock();
        } else {
            GuiUnlock();
        }
    }

    Rectangle deviceInfoSection = statusBarRect;
    deviceInfoSection.width = deviceInfoDrawerWidth;
    GuiStatusBar(deviceInfoSection, NULL);
    deviceInfoSection.x += PAD_8;

    char buffer[100];
    const char* icon = mainGuiState.selectedDevice && mainGuiState.selectedDevice->type == DEVICE_TYPE_ROUTER ? "#225#" : "#224#";
    sprintf(buffer, "%s %s", icon, mainGuiState.selectedDevice ? mainGuiState.selectedDevice->address : "No device selected");
    if (mainGuiState.deviceDrawerActive) GuiSetState(STATE_PRESSED);
    if (GuiLabelButton(deviceInfoSection, buffer) && mainGuiState.selectedDevice) {
        mainGuiState.deviceDrawerActive = !mainGuiState.deviceDrawerActive;
    }
    GuiSetState(STATE_NORMAL);
}

static void render_left_navbar() {
    int screenHeight = GetScreenHeight();
    Vector2 mousePos = GetMousePosition();

    Rectangle rectangle = (Rectangle){
        0, UI_TOP_NAVBAR_HEIGHT,
        UI_LEFT_SIDEBAR_WIDTH, screenHeight-UI_TOP_NAVBAR_HEIGHT-UI_STATUS_BAR_HEIGHT
    };
    GuiPanel(rectangle, NULL);

    Rectangle btnRect = (Rectangle){6, UI_TOP_NAVBAR_HEIGHT + PAD_8, 24, 24};

    bool isInitToolwindow = false;

    if (GuiButton(btnRect, "#241#")) {
        if (!mainGuiState.toolWindowState.activeToolWindow) isInitToolwindow = true;
        mainGuiState.activeLeftPanelMode = LEFT_PANEL_MODE_AREA_VIEWER;
        mainGuiState.toolWindowState.activeToolWindow = TOOLWINDOW_NETWORK_MAP;
    }
    if (CheckCollisionPointRec(mousePos, btnRect)) {
        GuiTooltipCustom(btnRect, "Network Map");
    }
    btnRect.y += 30;
    if (GuiButton(btnRect, "#208#")) {
        if (!mainGuiState.toolWindowState.activeToolWindow) isInitToolwindow = true;
        mainGuiState.toolWindowState.activeToolWindow = TOOLWINDOW_WORMS;
    }
    if (CheckCollisionPointRec(mousePos, btnRect)) {
        GuiTooltipCustom(btnRect, "w0rms");
    }

    if (isInitToolwindow) {
        mainAreaViewerWindowState.camera.offset.x -= UI_LEFT_TOOLWINDOW_WIDTH * mainAreaViewerWindowState.camera.zoom;
        init_tool_window(&mainGuiState.toolWindowState);
    }
}

static void render_top_navbar() {
    int screenWidth = GetScreenWidth();

    Rectangle rectangle = (Rectangle){0, 1, screenWidth, UI_TOP_NAVBAR_HEIGHT};
    GuiPanel(rectangle, NULL);

    Rectangle topbarRect = (Rectangle){4, 5, 150, UI_TOP_NAVBAR_HEIGHT - 9};
    Rectangle textRect = (Rectangle){40, 8, 140, 14};
    GuiSetState(STATE_DISABLED); GuiPanel(topbarRect, NULL); GuiSetState(STATE_NORMAL);
    GuiDrawIcon(244, topbarRect.x + PAD_8, topbarRect.y + PAD_8, 1, WHITE);
    GuiLabel(textRect, "nightcity.metro");
    textRect.y += 12;
    GuiLabel(textRect, "23:00 - Day 1");
    textRect.y -= 12;

    char buffer[100];
    topbarRect.x += 154;
    textRect.x += 154;
    topbarRect.width = 300;
    GuiSetState(STATE_DISABLED); GuiPanel(topbarRect, NULL); GuiSetState(STATE_NORMAL);
    GuiDrawIcon(247, topbarRect.x + PAD_8, topbarRect.y + PAD_8, 1, WHITE);
    sprintf(buffer, "%d targets", botnetState.numTargetDevices);
    GuiLabel(textRect, buffer);
    textRect.y += 12;
    sprintf(buffer, "%d hosts pwned", botnetState.targetsPwned);
    GuiLabel(textRect, buffer);
    textRect.y -= 12;
    textRect.x += 120;
    GuiLabel(textRect, botnetState.state == BOTNET_STATE_ACTIVE ? "Attack in progress" : "Botnet inactive");
    textRect.y += 14;
    float progress = botnetState.state == BOTNET_STATE_ACTIVE ? botnetState.targetsCompleted : 0;
    float total =  botnetState.state == BOTNET_STATE_ACTIVE ? botnetState.numTargetDevices : 100;
    GuiProgressBar((Rectangle){textRect.x, textRect.y, 130, 12}, NULL, NULL, &progress, 0, total);
    textRect.y -= 14;

    topbarRect.x += 304;
    textRect.x += 304 - 120;
    topbarRect.width = 120;
    GuiSetState(STATE_DISABLED); GuiPanel(topbarRect, NULL); GuiSetState(STATE_NORMAL);
    GuiDrawIcon(248, topbarRect.x + PAD_8, topbarRect.y + PAD_8, 1, WHITE);
    GuiLabel(textRect, "Bitcredits");
    textRect.y += 12;
    sprintf(buffer, "%d", worldState.bitCredits);
    GuiLabel(textRect, buffer);

    topbarRect.x = GetScreenWidth() - 154;
    topbarRect.width = 150;
    GuiSetState(STATE_DISABLED); GuiPanel(topbarRect, NULL); GuiSetState(STATE_NORMAL);

    Rectangle btnRect = (Rectangle){screenWidth-32, 10, 24, 24};
    textRect = btnRect;
    textRect.width = 50;
    GuiButton(btnRect, "#159#");
    btnRect.x -= 28;
    GuiButton(btnRect, "#141#");
    btnRect.x -= 28;
    GuiButton(btnRect, "#002#");
    textRect.x -= 110;
    GuiLabel(textRect, "Saved");
}

void gui_load_area_view_left_panel(Area* area) {
    mainAreaViewerWindowState = init_area_viewer_window(area, mainGuiState.leftPanelRect);
    mainGuiState.activeLeftPanelMode = LEFT_PANEL_MODE_AREA_VIEWER;
}

void gui_load_device_info_panel(Device* device) {
    if (!device) {
        mainGuiState.deviceDrawerActive = false;
    } else if (mainGuiState.selectedDevice == device) {
        mainGuiState.deviceDrawerActive = true;
    }
    mainGuiState.selectedDevice = device;
}

void gui_set_active_worm(Worm* worm) {
    mainGuiState.selectedWorm = worm;
}

Device* gui_get_selected_device() {
    return mainGuiState.selectedDevice;
}
