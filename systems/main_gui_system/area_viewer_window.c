#include <string.h>
#include <stdio.h>
#include "glib.h"
#include "raylib.h"
#include "../../lib/raygui.h"
#include "area_viewer_window.h"
#include "isometric_map_rendering.h"
#include "../../components/components.h"
#include "../../components/component_registry.h"
#include "../../graphics/tiles.h"

#define STATUSBAR_HEIGHT 18
#define UI_COMPONENT_PADDING 8
#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 512

AreaViewerWindowState init_area_viewer_window(Area* area, Rectangle rect) {
    AreaViewerWindowState state;

    // Init window data
    state.window.windowBounds = rect;
    state.window.windowActive = true;
    state.window.supportDrag = true;
    state.window.dragMode = false;
    state.window.panOffset = (Vector2){ 0, 0 };
    sprintf(state.window.windowTitle, "#100# %s", area->areaName);

    // Area
    state.area = area;

    // Area rendering
    state.camera.target = (Vector2){0, 0};
    state.camera.offset = (Vector2){state.window.windowBounds.width/2, state.window.windowBounds.height/3};
    state.camera.rotation = 0.0f;
    state.camera.zoom = 0.5f;

    state.selectedDevice = NULL;

    return state;
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
    Vector2 mousePos = GetMousePosition();
    Vector2 clickedTile = isometric_map_global_to_local(state->window.windowBounds, state->camera.offset,
                                                        mousePos.x, mousePos.y, state->camera.zoom);
    clickedTile = (Vector2){(int)clickedTile.x, (int)clickedTile.y};

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (int i = 0; i < state->area->numEntities; i++) {
            Position* position = g_hash_table_lookup(componentRegistry.positions, state->area->entities[i]);
            if (position->coord.x != clickedTile.x || position->coord.y != clickedTile.y) continue;

            Device* device = g_hash_table_lookup(componentRegistry.devices, state->area->entities[i]);
            if (device != NULL && device->visible) {
                state->selectedDevice = device;
            }
        }
    }
}

int update_area_viewer_window(AreaViewerWindowState* state) {
    if (!state->window.windowActive) return 0;

    update_area_viewer_camera_control(state);
    update_area_viewer_selected_device(state);

    // Update viewport
    state->viewport = (Rectangle){
        state->window.windowBounds.x + UI_COMPONENT_PADDING,
        state->window.windowBounds.y + UI_COMPONENT_PADDING,
        state->window.windowBounds.width - (UI_COMPONENT_PADDING*2),
        state->window.windowBounds.height - (UI_COMPONENT_PADDING*2),
    };

    return update_window(&state->window);
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

void draw_device_sprite(AreaViewerWindowState* state, SpriteRect spriteRect, Vector2 coord) {
    Vector2 globalCoord = isometric_map_local_to_global(state->window.windowBounds, coord.x, coord.y, state->camera.zoom);
    float globalX = globalCoord.x;
    float globalY = globalCoord.y;

    Rectangle rect = spriteRect.rect;
    Vector2 offset = spriteRect.offset;
    offset = (Vector2){-rect.width/2-offset.x, -offset.y};
    Vector2 position = (Vector2){globalX + offset.x, globalY + offset.y};

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
        Vector2 globalCoord = isometric_map_local_to_global(state->window.windowBounds, position->coord.x, position->coord.y, state->camera.zoom);
        float globalX = globalCoord.x;
        float globalY = globalCoord.y;
        DrawText("L", globalX, globalY, 10, BLUE);
    }
}

/**
 * Renders connection to parent
 *
 * @param state
 * @param entityId Entity ID of connection
 * @param connection The connection
 */
void render_area_connection(AreaViewerWindowState* state, char* entityId, Connection* connection) {
    char* fromEntity = entityId;

    char* toEntity = connection->parentEntityId;
    if (!toEntity || strlen(toEntity) == 0) return;

    Vector2 fromCoord;
    if (strcmp(fromEntity, state->area->zoneRouterEntityId) == 0) {
       fromCoord = isometric_map_local_to_global(state->window.windowBounds, state->area->zoneRouterCoord.x,
                                                 state->area->zoneRouterCoord.y, state->camera.zoom);
    } else {
        Position* fromPos = (Position*)g_hash_table_lookup(componentRegistry.positions, fromEntity);
        fromCoord = isometric_map_local_to_global(state->window.windowBounds, fromPos->coord.x, fromPos->coord.y,
                                                  state->camera.zoom);
    }
    Vector2 toCoord;
    if (strcmp(toEntity, state->area->zoneRouterEntityId) == 0) {
        toCoord = isometric_map_local_to_global(state->window.windowBounds, state->area->zoneRouterCoord.x,
                                                state->area->zoneRouterCoord.y, state->camera.zoom);
    } else {
        Position* toPos = (Position*)g_hash_table_lookup(componentRegistry.positions, toEntity);
        toCoord = isometric_map_local_to_global(state->window.windowBounds, toPos->coord.x, toPos->coord.y,
                                                  state->camera.zoom);
    }

    fromCoord.y += SPRITE_Y_SCALE / 2;
    toCoord.y += SPRITE_Y_SCALE / 2;

    // Animate connection if either direction is "active" (receiving packet in either direction)
    Connection* toConnection = g_hash_table_lookup(componentRegistry.connections, toEntity);
    if (strcmp(connection->activeEntityId, toEntity) == 0 || (toConnection && strcmp(toConnection->activeEntityId, fromEntity) == 0)) {
        DrawLineEx(fromCoord, toCoord, 3, GREEN);
    } else {
        DrawLineEx(fromCoord, toCoord, 3, WHITE);
    }
}

void render_area_window_selected_device(AreaViewerWindowState* state) {
    if (state->selectedDevice == NULL) return;

    char* entity_id = state->selectedDevice->entityId;
    Sprite* sprite = (Sprite*)g_hash_table_lookup(componentRegistry.sprites, entity_id);

    // Render Sprite
    Position* position = (Position*)g_hash_table_lookup(componentRegistry.positions, entity_id);
    Vector2 coord = position->coord;
    SpriteRect sprite_rect = spriteSheet[sprite->spriteId];

    Vector2 globalCoord = isometric_map_local_to_global(state->window.windowBounds, coord.x, coord.y, state->camera.zoom);
    float globalX = globalCoord.x;
    float globalY = globalCoord.y;

    Vector2 offset = sprite_rect.offset;
    offset = (Vector2){-sprite_rect.rect.width/2-offset.x, -offset.y};

    BeginMode2D(state->camera);

    DrawTextureRec(textureSpriteSheet, spriteSheet[SPRITE_SELECTED].rect, (Vector2){globalX + offset.x, globalY + offset.y}, WHITE);
    DrawTextureRec(textureSpriteSheet, sprite_rect.rect, (Vector2){globalX + offset.x, globalY + offset.y}, WHITE);

    EndMode2D();
}

void render_area_packet(AreaViewerWindowState* state, char* entityId, PacketBuffer* packetBuffer) {
    Position* fromPos = (Position*)g_hash_table_lookup(componentRegistry.positions, entityId);
    Vector2 globalCoord = isometric_map_local_to_global(state->window.windowBounds, fromPos->coord.x, fromPos->coord.y, state->camera.zoom);

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

    // For debugging purposes:
//    for (int i = 0; i < currentArea->numEntities; i++) {
//        char* entity_id = currentArea->entities[i];
//        PacketBuffer* pbuffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entity_id);
//        if (pbuffer == NULL) continue;
//        render_area_packet(state, entity_id, pbuffer);
//    }

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
    Vector2 currentTile = isometric_map_global_to_local(state->window.windowBounds, state->camera.offset,
                                                        mousePos.x, mousePos.y, state->camera.zoom);
    currentTile = (Vector2){(float)currentTile.x, (float)currentTile.y};

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

    Vector2 localPos = isometric_map_global_to_local(state->window.windowBounds, state->camera.offset,
                                                     mousePos.x, mousePos.y, state->camera.zoom);
    sprintf(buffer, "Mouse Local: (%d, %d)", (int)localPos.x, (int)localPos.y);
    DrawText(buffer, state->viewport.x+5, state->viewport.height+state->viewport.y-24, 10, WHITE);

    sprintf(buffer, "Camera Offset: (%d, %d)", (int)state->camera.offset.x, (int)state->camera.offset.y);
    DrawText(buffer, state->viewport.x+5, state->viewport.height+state->viewport.y-12, 10, WHITE);
}

/**
 * Returns 1 when closing
 */
int render_area_viewer_window(AreaViewerWindowState* state) {
    if (!state->window.windowActive) return 0;
    if (!state->area) return 0;

    GuiPanel(state->window.windowBounds, state->area->areaName);

    BeginScissorMode(state->viewport.x, state->viewport.y+TITLEBAR_HEIGHT, state->viewport.width, state->viewport.height-TITLEBAR_HEIGHT);

    BeginMode2D(state->camera);
    isometric_map_draw_grid(state->window.windowBounds, state->area->width, state->area->height, state->camera.zoom);
    isometric_map_render_tiles(state->window.windowBounds, state->area->width, state->area->height, state->camera.zoom);
    EndMode2D();

    render_area(state);
    render_area_window_selected_device(state);

    _area_viewer_render_device_mouseover_hover(state);
    _area_viewer_draw_mouse_coords(state);

    EndScissorMode();

    return !state->window.windowActive;
}
