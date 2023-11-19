#include <stdio.h>
#include <string.h>
#include "worms_window.h"
#include "raylib.h"
#include "../../lib/log/log.h"
#include "../../lib/raygui.h"
#include "../../lib/text_rectangle_bounds.h"
#include "../../store/worms.h"
#include "../../world/world_state.h"

#define WORM_WINDOW_WIDTH 800
#define WORM_WINDOW_HEIGHT 600
#define TITLEBAR_HEIGHT 24
#define TOP_SECTION_HEIGHT 48
#define PAD_8 8
#define UI_ICON_PADDING 16
#define WORM_LEFT_PANEL 220

typedef enum {
    WormsPaletteTop,
    WormsPaletteScanner,
    WormsPaletteCredential,
    WormsPaletteExploits,
    WormsPaletteFile,
    WormsPaletteTrojan,
} WormsPaletteMode;

const char* WormsPaletteTitle[] = {
    "Palette",
    "Palette > Scanners",
    "Palette > Cred Attacks",
    "Palette > Remote Exploits",
    "Palette > Malicious Files",
    "Palette > Trojan",
};

typedef struct {
    bool isOpen;

    bool wormNameEditMode;
    Worm worm;

    WormsPaletteMode paletteMode;

    bool paletteItemSelected;
    WormSlot selectedPaletteItem;
} WormsWindowState;

WormsWindowState wormsWindowState;

int worms_window_gui_draw_option(int x, int y, int icon, const char* title, const char* subtitle, const char* description);
int worms_window_gui_draw_exploit_option(int x, int y, Exploit exploit);
void worms_window_render_palette(int x, int y);
void worms_window_top_section(int x, int y, int width);
void worms_window_draw_worm_slot(int x, int y, WormSlot wormSlot);
void worms_window_capabilities_pane(int x, int y);

void init_worms_window() {
    strcpy(wormsWindowState.worm.wormName, "myw0rm");
    wormsWindowState.wormNameEditMode = false;
    wormsWindowState.paletteMode = WormsPaletteTop;

    wormsWindowState.paletteItemSelected = false;
    wormsWindowState.selectedPaletteItem.type = WormSlotEmpty;

    wormsWindowState.worm.numSlots = 9;
    for (int i = 0; i < 12; i++) {
        wormsWindowState.worm.slots[i].type = (i < wormsWindowState.worm.numSlots) ? WormSlotEmpty : WormSlotLocked;
    }

    wormsWindowState.isOpen = true;
}

void render_worms_window() {
    GuiUnlock();
    if (!wormsWindowState.isOpen) return;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int x = (screenWidth / 2)  - (WORM_WINDOW_WIDTH / 2);
    int y = (screenHeight / 2) - (WORM_WINDOW_HEIGHT / 2);

    // Black out main GUI
    DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.5f));

    // Pop up window
    Rectangle windowRect = { x, y, WORM_WINDOW_WIDTH, WORM_WINDOW_HEIGHT };
    if (GuiWindowBox(windowRect, "#208#New w0rm")) {
        wormsWindowState.isOpen = false;
    }

    // Top section
    worms_window_top_section(x, y + TITLEBAR_HEIGHT - 1, windowRect.width);

    // Palette left panel
    int iconsX = x + PAD_8;
    int iconsY = y + TITLEBAR_HEIGHT * 2 + TOP_SECTION_HEIGHT + PAD_8 - 1;
    Rectangle leftPanelRect = { x+1, y + TITLEBAR_HEIGHT + TOP_SECTION_HEIGHT,
                                WORM_LEFT_PANEL, windowRect.height - TITLEBAR_HEIGHT - TOP_SECTION_HEIGHT };
    GuiPanel(leftPanelRect, WormsPaletteTitle[wormsWindowState.paletteMode]);
    worms_window_render_palette(iconsX, iconsY);

    // Main capabilities pane
    int capabilitiesHeadingY = y + TITLEBAR_HEIGHT + TOP_SECTION_HEIGHT + PAD_8 - 1;
    int capabilitiesX = x + 220 + 25;
    worms_window_capabilities_pane(capabilitiesX, capabilitiesHeadingY);

    // Drag and drop palette item
    if (wormsWindowState.paletteItemSelected) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            wormsWindowState.paletteItemSelected = false;
            wormsWindowState.selectedPaletteItem.type = WormSlotEmpty;
            wormsWindowState.selectedPaletteItem.content.pointer = NULL;
            log_debug("Drag n drop: mouse released, item reset");
        } else {
            Vector2 mousePos = GetMousePosition();
            if (wormsWindowState.selectedPaletteItem.type != WormSlotEmpty) {
                worms_window_draw_worm_slot(mousePos.x, mousePos.y, wormsWindowState.selectedPaletteItem);
            }
        }
    }

    GuiLock();
}

int worms_window_gui_draw_option(int x, int y, int icon, const char* title, const char* subtitle, const char* description) {
    Color labelColor = GetColor(GuiGetStyle(LABEL, TEXT_COLOR_NORMAL));
    Color highlightColor = GetColor(GuiGetStyle(LABEL, TEXT_COLOR_PRESSED));
    int iconTileSize = 48;
    float labelX = (float)x + (float)iconTileSize + PAD_8;

    Rectangle optionRect = { x, y, 204, 48 };
    bool hover = CheckCollisionPointRec(GetMousePosition(), optionRect);
    bool clicked = false;
    if (hover) {
        DrawRectangle(x, y, 204, 48, ColorAlpha(labelColor, 0.05f));

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            clicked = true;
        }
    }

    // Icon
    DrawRectangle(x, y, iconTileSize, iconTileSize, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_DISABLED)));
    GuiDrawIcon(icon, x+PAD_8, y+PAD_8, 2, hover ? highlightColor : labelColor);

    // Name of vuln
    DrawTextEx(GuiGetFont(), title, (Vector2){ labelX+1, y }, 14, 0, RAYWHITE);

    Rectangle labelRect = { labelX, y+14, WORM_LEFT_PANEL-iconTileSize-(PAD_8*2), 14 };
    GuiLabel(labelRect, subtitle);

    if (description) {
        DrawTextEx(GuiGetFont(), description, (Vector2){ labelX+1, y+28 }, 14, 0, ColorAlpha(labelColor, 0.5f));
    }

    return clicked;
}

void worms_window_gui_render_back_option(int x, int y) {
    if (worms_window_gui_draw_option(x, y, ICON_ARROW_LEFT_FILL, "", "Back", "Go back to main menu")) {
        wormsWindowState.paletteMode = WormsPaletteTop;
    }
}

int worms_window_gui_draw_exploit_option(int x, int y, Exploit exploit) {
    char title[100];
    sprintf(title, "%s %.2f", exploit.programName, exploit.programVersion/100.0f);
    return worms_window_gui_draw_option(x, y, 211, title, EXPLOIT_TYPE_LABEL(exploit.exploitType), exploit.id);
}

int worms_window_gui_draw_creds_option(int x, int y, CredDump credDump) {
    char buffer[100];
    sprintf(buffer, "%d creds", credDump.numCredentials);
    return worms_window_gui_draw_option(x, y, 220, credDump.name, credDump.shortDescription, buffer);
}

void worms_window_render_palette_top(int x, int y) {
    if (worms_window_gui_draw_option(x, y, 212, "Scan New Targets", "Host Discovery", "Scans nearby networks")) {
        wormsWindowState.paletteMode = WormsPaletteScanner;
    }
    y += 48 + PAD_8;
    if (worms_window_gui_draw_option(x, y, 210, "Credential Attacks", "Active Attack", "Tries known user/pass")) {
        wormsWindowState.paletteMode = WormsPaletteCredential;
    }
    y += 48 + PAD_8;
    if (worms_window_gui_draw_option(x, y, 211, "Remote Exploits", "Active Attack", "Exploit vulns remotely")) {
        wormsWindowState.paletteMode = WormsPaletteExploits;
    }
    y += 48 + PAD_8;
    if (worms_window_gui_draw_option(x, y, 215, "Malicious File", "File-Based Threat", "Vulns triggered by file")) {
        wormsWindowState.paletteMode = WormsPaletteFile;
    }
    y += 48 + PAD_8;
    if (worms_window_gui_draw_option(x, y, 219, "Trojan", "File-Based Threat", "Deliver w0rm in disguise")) {
        wormsWindowState.paletteMode = WormsPaletteTrojan;
    }
}

void worms_window_render_palette_scanner(int x, int y) {
    worms_window_gui_render_back_option(x, y);
}

void worms_window_render_palette_credential(int x, int y) {
    worms_window_gui_render_back_option(x, y);
    y += 48 + PAD_8;

    for (int i = 0; i < worldState.numCredDumps; i++) {
        if (worms_window_gui_draw_creds_option(x, y, worldState.credDumps[i])) {
            wormsWindowState.paletteItemSelected = true;
            wormsWindowState.selectedPaletteItem.type = WormSlotCredentialAttack;
            wormsWindowState.selectedPaletteItem.content.credDump = &worldState.credDumps[i];
            log_debug("Drag n drop: selected creds %s", wormsWindowState.selectedPaletteItem.content.credDump->name);
        }
        y += 48 + PAD_8;
    }
}

void worms_window_render_palette_exploits(int x, int y) {
    worms_window_gui_render_back_option(x, y);
    y += 48 + PAD_8;

    for (int i = 0; i < worldState.numExploits; i++) {
        if (worldState.exploits[i].exploitScope != ExploitScopeRemote) continue;
        if (worms_window_gui_draw_exploit_option(x, y, worldState.exploits[i])) {
            wormsWindowState.paletteItemSelected = true;
            wormsWindowState.selectedPaletteItem.type = WormSlotRemoteExploit;
            wormsWindowState.selectedPaletteItem.content.exploit = &worldState.exploits[i];
            log_debug("Drag n drop: selected exploit %s", wormsWindowState.selectedPaletteItem.content.exploit->programName);
        }
        y += 48 + PAD_8;
    }
}

void worms_window_render_palette_file(int x, int y) {
    worms_window_gui_render_back_option(x, y);
    y += 48 + PAD_8;

    for (int i = 0; i < worldState.numExploits; i++) {
        if (worldState.exploits[i].exploitScope != ExploitScopeLocalAuto) continue;
        if (worldState.exploits[i].exploitScope != ExploitScopeLocalOpen) continue;
        worms_window_gui_draw_exploit_option(x, y, worldState.exploits[i]);
        y += 48 + PAD_8;
    }
}

void worms_window_render_palette_trojan(int x, int y) {
    worms_window_gui_render_back_option(x, y);
}

void worms_window_render_palette(int x, int y) {
    switch (wormsWindowState.paletteMode) {
        case WormsPaletteTop:        worms_window_render_palette_top(x, y);          break;
        case WormsPaletteScanner:    worms_window_render_palette_scanner(x, y);      break;
        case WormsPaletteCredential: worms_window_render_palette_credential(x, y);   break;
        case WormsPaletteExploits:   worms_window_render_palette_exploits(x, y);     break;
        case WormsPaletteFile:       worms_window_render_palette_file(x, y);         break;
        case WormsPaletteTrojan:     worms_window_render_palette_trojan(x, y);       break;
    }
}

void worms_window_top_section(int x, int y, int width) {
    Color labelColor = GetColor(GuiGetStyle(LABEL, TEXT_COLOR_NORMAL));
    Rectangle topSectionRect = { x, y, width, TOP_SECTION_HEIGHT + 1 };
    GuiPanel(topSectionRect, NULL);

    int paddedX = x + PAD_8+4;
    int paddedY = y + PAD_8;
    GuiLabel((Rectangle){paddedX,paddedY+PAD_8,100,14}, "Name:");
    paddedX += 45;
    if (GuiTextBox((Rectangle){paddedX,paddedY+4,160,24}, wormsWindowState.worm.wormName, 20, wormsWindowState.wormNameEditMode)) {
        wormsWindowState.wormNameEditMode = !wormsWindowState.wormNameEditMode;
    }
    paddedX += 200;
    GuiLabel((Rectangle){paddedX,paddedY+PAD_8,100,14}, "Icon:");
    paddedX += 40;
    GuiDrawIcon(208, paddedX, paddedY, 2, labelColor);

    if (GuiLabelButton((Rectangle){ x+WORM_WINDOW_WIDTH-64-52-PAD_8*4, y+2+PAD_8, 52, 28 }, "Cancel")) {
        wormsWindowState.isOpen = false;
    }
    if (GuiButton((Rectangle){ x+WORM_WINDOW_WIDTH-64-PAD_8*3, y+2+PAD_8, 77, 28 }, "#002# Save")) {
        wormsWindowState.isOpen = false;
        worldState.worms[worldState.numWorms++] = wormsWindowState.worm;
    }
}

void worms_window_draw_worm_slot(int x, int y, WormSlot wormSlot) {
    Color labelColor = GetColor(GuiGetStyle(LABEL, TEXT_COLOR_NORMAL));
    Color labelColorFaded = ColorAlpha(GetColor(GuiGetStyle(LABEL, TEXT_COLOR_NORMAL)), 0.5f);
    int slotSize = 120;
    Rectangle slotTextRect = { x+4, y, slotSize-8, slotSize-4 };
    char buffer[1000];

    switch(wormSlot.type) {
        case WormSlotEmpty:
            break;
        case WormSlotLocked:
            GuiDrawIcon(214, x+slotSize-64, y+slotSize-64, 4, labelColorFaded);
            break;
        case WormSlotRemoteExploit: {
            Exploit exploit = *wormSlot.content.exploit;
            sprintf(buffer, "%s\n%s %.2f\n%s", exploit.id, exploit.programName, exploit.programVersion/100.0f,
                    EXPLOIT_TYPE_LABEL(exploit.exploitType));
            DrawTextBoxed(GuiGetFont(), buffer, slotTextRect, 14, 0, true, labelColor);
            GuiDrawIcon(211, x + slotSize - 64, y + slotSize - 64, 4, labelColorFaded);
            break;
        }
        case WormSlotCredentialAttack: {
            CredDump credDump = *wormSlot.content.credDump;
            sprintf(buffer, "%s\n%d creds\n%s", credDump.name, credDump.numCredentials, credDump.shortDescription);
            DrawTextBoxed(GuiGetFont(), buffer, slotTextRect, 14, 0, true, labelColor);
            GuiDrawIcon(220, x + slotSize - 64, y + slotSize - 64, 4, labelColorFaded);
            break;
        }
        default:
            break;
    }
}

void worms_window_render_capability_slot(int x, int y, int slotNum) {
    WormSlot currentWormSlot = wormsWindowState.worm.slots[slotNum];
    int slotSize = 120;

    Rectangle boundRect = { x, y, slotSize, slotSize };
    bool isDropTarget = currentWormSlot.type != WormSlotLocked && wormsWindowState.paletteItemSelected
            && CheckCollisionPointRec(GetMousePosition(), boundRect);

    if (isDropTarget) {
        DrawRectangle(x, y, slotSize, slotSize, ColorAlpha(BLACK, 0.4f));
        DrawRectangleLines(x, y, slotSize, slotSize, RAYWHITE);

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            wormsWindowState.worm.slots[slotNum] = wormsWindowState.selectedPaletteItem;
            wormsWindowState.paletteItemSelected = false;
            wormsWindowState.selectedPaletteItem.type = WormSlotEmpty;
        }
    } else {
        DrawRectangle(x, y, slotSize, slotSize, ColorAlpha(BLACK, 0.5f));
    }

    worms_window_draw_worm_slot(x, y, currentWormSlot);
}

void worms_window_capabilities_pane(int x, int y) {
    int capabilitiesY = y + 48;

    char buffer[100];
    sprintf(buffer, "%s's Capabilities", wormsWindowState.worm.wormName);
    DrawTextEx(GuiGetFont(), buffer, (Vector2){ x, y+PAD_8 }, 28, 0, RAYWHITE);

    y = capabilitiesY;

    for (int i = 0; i < 12; i++) {
        worms_window_render_capability_slot(x+(i%4*136), y+(i/4*136), i);
    }
}
