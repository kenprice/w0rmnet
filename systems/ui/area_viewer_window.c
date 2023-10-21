#include <string.h>
#include <stdio.h>
#include "glib.h"
#include "raylib.h"
#include "area_viewer_window.h"
#include "../../components/components.h"
#include "../../components/component_registry.h"
#include "../../graphics/tiles.h"
#include "../../lib/raygui.h"

#define TITLEBAR_HEIGHT 24
#define STATUSBAR_HEIGHT 18
#define UI_COMPONENT_PADDING 8
#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 512

AreaViewerWindowState init_area_viewer_window(Area* area) {
    AreaViewerWindowState state;

    // Init window data
    state.window.windowBounds = (Rectangle){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    state.window.windowActive = true;
    state.window.supportDrag = true;
    state.window.dragMode = false;
    state.window.panOffset = (Vector2){ 0, 0 };
    sprintf(state.window.windowTitle, "#100# %s", area->areaName);

    // Area
    state.area = area;

    // Area rendering
    state.camera.target = (Vector2){0, 0};
    state.camera.offset = (Vector2){state.window.windowBounds.width/2, state.window.windowBounds.height/2};
    state.camera.rotation = 0.0f;
    state.camera.zoom = 0.5f;

    state.selectedDevice = NULL;
    state.deviceInfoWindowState = init_device_info_window(NULL);

    return state;
}

Vector2 window_convert_local_to_global(AreaViewerWindowState* state, float x, float y) {
    float isoX = state->window.windowBounds.x / state->camera.zoom;
    float isoY = state->window.windowBounds.y / state->camera.zoom;
    float isoW = (float)SPRITE_X_SCALE / 2;
    float isoH = (float)SPRITE_Y_SCALE / 2;
    float globalX = isoX + (x - y) * isoW;
    float globalY = isoY + (x + y) * isoH;
    return (Vector2){globalX, globalY};
}

Vector2 window_convert_global_to_local(AreaViewerWindowState* state, float x, float y) {
    float scaledX = (x - state->window.windowBounds.x) / state->camera.zoom;
    float scaledY = (y - state->window.windowBounds.y) / state->camera.zoom;
    float isoX = state->camera.offset.x / state->camera.zoom;
    float isoY = state->camera.offset.y / state->camera.zoom;
    float isoW = (float)SPRITE_X_SCALE / 2;
    float isoH = (float)SPRITE_Y_SCALE / 2;
    float localX = ((scaledY - isoY) / isoH + (scaledX - isoX) / isoW) / 2;
    float localY = ((scaledY - isoY) / isoH - (scaledX - isoX) / isoW) / 2;
    return (Vector2){localX, localY};
}

void update_area_viewer_camera_control(AreaViewerWindowState* state) {
    if (!CheckCollisionPointRec(GetMousePosition(), state->window.windowBounds)) return;

    int offset = 10;

    if (IsKeyDown(KEY_D)) state->camera.offset.x -= offset;
    else if (IsKeyDown(KEY_A)) state->camera.offset.x += offset;
    if (IsKeyDown(KEY_W)) state->camera.offset.y += offset;
    else if (IsKeyDown(KEY_S)) state->camera.offset.y -= offset;


}

void update_area_viewer_selected_device(AreaViewerWindowState* state) {
    if (update_device_info_window(&state->deviceInfoWindowState)) return;

    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 clickedTile = window_convert_global_to_local(state, mousePos.x, mousePos.y);
        clickedTile = (Vector2){(int)clickedTile.x, (int)clickedTile.y};

        for (int i = 0; i < state->area->numEntities; i++) {
            Position* position = g_hash_table_lookup(componentRegistry.positions, state->area->entities[i]);
            if (position->coord.x != clickedTile.x || position->coord.y != clickedTile.y) continue;

            Device* device = g_hash_table_lookup(componentRegistry.devices, state->area->entities[i]);
            if (device != NULL && device->visible) {
                state->selectedDevice = device;

                char buffer[50] = "";
                strcat(buffer, (device == NULL || device->type != DEVICE_TYPE_ROUTER) ? "#224#" : "#225#");
                state->deviceInfoWindowState.device = device;
                strcat(buffer, (device != NULL ? device->name : "Device"));
                strcpy(state->deviceInfoWindowState.windowTitle, buffer);
                state->deviceInfoWindowState.windowActive = true;
                break;
            }
        }
    }
}

int update_area_viewer_window(AreaViewerWindowState* state) {
    if (!state->window.windowActive) return 0;

    Vector2 mousePosition = GetMousePosition();

    update_area_viewer_camera_control(state);
    update_area_viewer_selected_device(state);

    // Update viewport
    state->viewport = (Rectangle){
        state->window.windowBounds.x + UI_COMPONENT_PADDING,
        state->window.windowBounds.y + TITLEBAR_HEIGHT + UI_COMPONENT_PADDING,
        state->window.windowBounds.width - (UI_COMPONENT_PADDING*2),
        state->window.windowBounds.height - TITLEBAR_HEIGHT - (UI_COMPONENT_PADDING*2),
    };

    // Update window drag
    if (state->window.supportDrag) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Window can be dragged from the top window bar
            if (CheckCollisionPointRec(mousePosition, (Rectangle) {state->window.windowBounds.x, state->window.windowBounds.y,
                                                                   (float) state->window.windowBounds.width,
                                                                   TITLEBAR_HEIGHT})) {
                state->window.dragMode = true;
                state->window.panOffset.x = mousePosition.x - state->window.windowBounds.x;
                state->window.panOffset.y = mousePosition.y - state->window.windowBounds.y;
            }
        }

        if (state->window.dragMode) {
            state->window.windowBounds.x = (mousePosition.x - state->window.panOffset.x);
            state->window.windowBounds.y = (mousePosition.y - state->window.panOffset.y);

            // Check screen limits to avoid moving out of screen
            if (state->window.windowBounds.x < 0) state->window.windowBounds.x = 0;
            else if (state->window.windowBounds.x > (GetScreenWidth() - state->window.windowBounds.width)) state->window.windowBounds.x =
                                                                                                     GetScreenWidth() -
                                                                                                     state->window.windowBounds.width;

            if (state->window.windowBounds.y < 0) state->window.windowBounds.y = 0;
            else if (state->window.windowBounds.y > (GetScreenHeight() - state->window.windowBounds.height)) state->window.windowBounds.y =
                                                                                                       GetScreenHeight() -
                                                                                                       state->window.windowBounds.height;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) state->window.dragMode = false;
        }
    }

    return CheckCollisionPointRec(mousePosition, state->window.windowBounds);
}

void render_window(AreaViewerWindowState* state) {
    state->window.windowActive = !GuiWindowBox(state->window.windowBounds, state->window.windowTitle);

    Rectangle viewportRect = state->viewport;

    // Grid funnn
    int color = GuiGetStyle(DEFAULT, LINE_COLOR);
    GuiSetStyle(DEFAULT, LINE_COLOR, GuiGetStyle(DEFAULT, BORDER_COLOR_DISABLED));
    GuiGrid(viewportRect, NULL, 16, 2, NULL);
    GuiSetStyle(DEFAULT, LINE_COLOR, color);

    // All of this just to add another button to the title lmao
    int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
    int tempTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    Rectangle statusBar = { state->window.windowBounds.x, state->window.windowBounds.y, state->window.windowBounds.width, (float)TITLEBAR_HEIGHT };
    Rectangle closeButtonRec = { statusBar.x + statusBar.width - GuiGetStyle(STATUSBAR, BORDER_WIDTH) - 40,
                                 statusBar.y + TITLEBAR_HEIGHT/2.0f - 18.0f/2.0f, 18, 18 };
    GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiButton(closeButtonRec, GuiIconText(ICON_CROSS_SMALL, NULL));
    GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlignment);
}

void render_area_tiles(AreaViewerWindowState* state) {
    float isoX = state->window.windowBounds.x / state->camera.zoom;
    float isoY = state->window.windowBounds.y / state->camera.zoom;
    int isoW = SPRITE_X_SCALE / 2;
    int isoH = SPRITE_Y_SCALE / 2;

    int numXTiles = state->area->width;
    int numYTiles = state->area->height;

    for (int y = 0; y < numYTiles; y++) {
        for (int x = 0; x < numXTiles; x++) {
            int globalX = isoX + (x - y) * isoW;
            int globalY = isoY + (x + y) * isoH;

            Vector2 offset = tileSheet[0].offset;
            offset = (Vector2){-tileSheet[0].rect.width / 2 - offset.x, -offset.y};

            DrawTextureRec(textureTileSheet, tileSheet[0].rect, (Vector2){globalX + offset.x, globalY + offset.y}, WHITE);
        }
    }
}

void draw_device_sprite(AreaViewerWindowState* state, SpriteRect sprite_rect, Vector2 coord) {
    Vector2 global_coord = window_convert_local_to_global(state, coord.x, coord.y);
    float global_x = global_coord.x;
    float global_y = global_coord.y;

    Rectangle rect = sprite_rect.rect;
    Vector2 offset = sprite_rect.offset;
    offset = (Vector2){-rect.width/2-offset.x, -offset.y};
    Vector2 position = (Vector2){global_x+offset.x, global_y+offset.y};

    DrawTextureRec(textureSpriteSheet, rect, position, WHITE);
}

void render_area_device_sprites(AreaViewerWindowState* state, char* entityId, Device* device) {
    Sprite* sprite = (Sprite*)g_hash_table_lookup(componentRegistry.sprites, entityId);

    // Render Sprite
    Position* position = (Position*)g_hash_table_lookup(componentRegistry.positions, entityId);

    if (!device->visible) {
        draw_device_sprite(state, spriteSheet[SPRITE_UNKNOWN], position->coord);
        return;
    } else {
        draw_device_sprite(state, spriteSheet[sprite->spriteId], position->coord);
    }

    if (device->owner != DEVICE_OWNER_PLAYER) {
        Vector2 global_coord = window_convert_local_to_global(state, position->coord.x, position->coord.y);
        float global_x = global_coord.x;
        float global_y = global_coord.y;
        DrawText("L", global_x, global_y, 10, BLUE);
    }
}

void render_area_connection(AreaViewerWindowState* state, char* entityId, Connection* connection) {
    char* fromEntity = entityId;

    char* toEntity = connection->parentEntityId;
    if (!toEntity || strlen(toEntity) == 0) return;

    Vector2 fromCoord;
    if (strcmp(fromEntity, state->area->zoneRouterEntityId) == 0) {
        fromCoord = window_convert_local_to_global(state, state->area->zoneRouterCoord.x, state->area->zoneRouterCoord.y);
    } else {
        Position* fromPos = (Position*)g_hash_table_lookup(componentRegistry.positions, fromEntity);
        fromCoord = window_convert_local_to_global(state, fromPos->coord.x, fromPos->coord.y);
    }
    Vector2 toCoord;
    if (strcmp(toEntity, state->area->zoneRouterEntityId) == 0) {
        toCoord = window_convert_local_to_global(state, state->area->zoneRouterCoord.x, state->area->zoneRouterCoord.y);
    } else {
        Position* toPos = (Position*)g_hash_table_lookup(componentRegistry.positions, toEntity);
        toCoord = window_convert_local_to_global(state, toPos->coord.x, toPos->coord.y);
    }

    fromCoord.y += SPRITE_Y_SCALE / 2;
    toCoord.y += SPRITE_Y_SCALE / 2;

    DrawLineEx(fromCoord, toCoord, 3, WHITE);
}

void render_area_window_selected_device(AreaViewerWindowState* state) {
    if (state->selectedDevice == NULL) return;

    char* entity_id = state->selectedDevice->entityId;
    Sprite* sprite = (Sprite*)g_hash_table_lookup(componentRegistry.sprites, entity_id);

    // Render Sprite
    Position* position = (Position*)g_hash_table_lookup(componentRegistry.positions, entity_id);
    Vector2 coord = position->coord;
    SpriteRect sprite_rect = spriteSheet[sprite->spriteId];

    Vector2 global_coord = window_convert_local_to_global(state, coord.x, coord.y);
    float global_x = global_coord.x;
    float global_y = global_coord.y;

    Vector2 offset = sprite_rect.offset;
    offset = (Vector2){-sprite_rect.rect.width/2-offset.x, -offset.y};

    BeginMode2D(state->camera);

    DrawTextureRec(textureSpriteSheet, spriteSheet[SPRITE_SELECTED].rect, (Vector2){global_x + offset.x, global_y + offset.y}, WHITE);
    DrawTextureRec(textureSpriteSheet, sprite_rect.rect, (Vector2){global_x + offset.x, global_y + offset.y}, WHITE);

    EndMode2D();
}

void render_area_packet(AreaViewerWindowState* state, char* entityId, PacketBuffer* packetBuffer) {
    Position* fromPos = (Position*)g_hash_table_lookup(componentRegistry.positions, entityId);
    Vector2 globalCoord = window_convert_local_to_global(state, fromPos->coord.x, fromPos->coord.y);

    // Render SEND messages
    PacketQueue q = packetBuffer->sendQ;
    int offset = 12;
    for (int i = q.tail; i < q.head; i++) {
        Packet* packet = q.packets[i];
        char message[100] = "";
        strcat(message, packet->message);
        strcat(message, " to ");
        strcat(message, packet->toAddress);
        DrawRectangle(globalCoord.x - 1, globalCoord.y + offset, MeasureText(message, 10) + 2, 10, BLACK);
        DrawText(message, globalCoord.x, globalCoord.y + offset, 10, GREEN);
        offset += 12;
    }
    // Render RECV messages
    q = packetBuffer->recvQ;
    for (int i = q.tail; i < q.head; i++) {
        Packet* packet = q.packets[i];
        char* message = packet->message;
        DrawRectangle(globalCoord.x - 1, globalCoord.y + offset, MeasureText(message, 10) + 2, 10, BLACK);
        DrawText(message, globalCoord.x, globalCoord.y + offset, 10, BLUE);
        offset += 12;
    }
}

void render_area(AreaViewerWindowState* state) {
    BeginMode2D(state->camera);

    render_area_tiles(state);

    Area* currentArea = state->area;

    for (int i = 0; i < currentArea->numEntities; i++) {
        char* entity_id = currentArea->entities[i];
        Connection* conn = (Connection*)g_hash_table_lookup(componentRegistry.connections, entity_id);
        if (conn == NULL) continue;
        render_area_connection(state, entity_id, conn);
    }

    for (int i = 0; i < currentArea->numEntities; i++) {
        char* entityId = currentArea->entities[i];
        Device* device = (Device*)g_hash_table_lookup(componentRegistry.devices, entityId);
        if (device == NULL) continue;
        render_area_device_sprites(state, entityId, device);
    }

    for (int i = 0; i < currentArea->numEntities; i++) {
        char* entity_id = currentArea->entities[i];
        PacketBuffer* pbuffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entity_id);
        if (pbuffer == NULL) continue;
        render_area_packet(state, entity_id, pbuffer);
    }

    EndMode2D();
}

void _area_viewer_draw_label(char* label, Vector2 coord) {
    int width = MeasureText(label, 10);
    DrawRectangle(coord.x-1, coord.y-11, width+2, 10, BLACK);
    DrawText(label, coord.x, coord.y-11, 10, GREEN);
}

void _area_viewer_draw_device_id(Device device, Vector2 coord) {
    int width = MeasureText(device.name, 10);
    DrawRectangle(coord.x-1, coord.y-11, width+2, 10, BLACK);
    DrawText(device.name, coord.x, coord.y - 11, 10, GREEN);
}

void _area_viewer_render_device_mouseover_hover(AreaViewerWindowState* state) {
    if (!CheckCollisionPointRec(GetMousePosition(), state->window.windowBounds)) return;

    Vector2 mousePos = GetMousePosition();
    Vector2 currentTile = window_convert_global_to_local(state, mousePos.x, mousePos.y);
    currentTile = (Vector2){(int)currentTile.x, (int)currentTile.y};

    for (int i = 0; i < state->area->numEntities; i++) {
        Position* position = g_hash_table_lookup(componentRegistry.positions, state->area->entities[i]);
        if (position->coord.x != currentTile.x || position->coord.y != currentTile.y) continue;

        Device* device = g_hash_table_lookup(componentRegistry.devices, state->area->entities[i]);
        if (device != NULL && is_entity_in_area(*state->area, device->entityId)) {
            if (device->visible) {
                _area_viewer_draw_device_id(*device, mousePos);
            } else {
                _area_viewer_draw_label("???", mousePos);
            }
            break;
        }
    }
}

void _area_viewer_draw_mouse_coords(AreaViewerWindowState* state) {
    Vector2 mousePos = GetMousePosition();
    char buffer[1000];

    Vector2 localPos = window_convert_global_to_local(state, mousePos.x, mousePos.y);
    sprintf(buffer, "Mouse Local: (%d,%d)", (int)localPos.x, (int)localPos.y);
    DrawText(buffer, state->viewport.x, state->viewport.y, 10, WHITE);

    sprintf(buffer, "Camera Offset: (%d,%d)", (int)state->camera.offset.x, (int)state->camera.offset.y);
    DrawText(buffer, state->viewport.x, state->viewport.y+12, 10, WHITE);
}

/**
 * Returns 1 when closing
 */
int render_area_viewer_window(AreaViewerWindowState* state) {
    if (!state->window.windowActive) return 0;
    if (!state->area) return 0;

    render_window(state);

    BeginScissorMode(state->viewport.x, state->viewport.y, state->viewport.width, state->viewport.height);
    render_area(state);
    render_area_window_selected_device(state);

    _area_viewer_render_device_mouseover_hover(state);
    _area_viewer_draw_mouse_coords(state);

    EndScissorMode();

    if (render_device_window(&state->deviceInfoWindowState)) {
        state->selectedDevice = NULL;
    }

    return !state->window.windowActive;
}
