#include <stdio.h>
#include "../lib/raygui.h"
#include "glib.h"
#include "device_ui_system.h"
#include "ui/device_info_window.h"
#include "utils/rendering.h"
#include "../components/component_registry.h"

#define PANEL_HEIGHT 200

Device* selectedDevice;
DeviceInfoWindowState deviceInfoWindowState;

void draw_popover(int x, int y, char* message) {
    DrawRectangle(x-1, y-12, MeasureText(message, 10)+2, 10, BLACK);
    DrawText(message, x, y-12, 10, GREEN);
}

void draw_label(char* label, Vector2 coord) {
    int width = MeasureText(label, 10);
    DrawRectangle(coord.x-1, coord.y-11, width+2, 10, BLACK);
    DrawText(label, coord.x, coord.y-11, 10, GREEN);
}

void draw_device_id(Device device, Vector2 coord) {
    int width = MeasureText(device.id, 10);
    DrawRectangle(coord.x-1, coord.y-11, width+2, 10, BLACK);
    DrawText(device.id, coord.x, coord.y-11, 10, GREEN);
}

void render_selected_device() {
    if (selectedDevice == NULL) return;

    char* entity_id = selectedDevice->entity_id;
    Sprite* sprite = (Sprite*)g_hash_table_lookup(component_registry.sprites, entity_id);

    // Render Sprite
    Position* position = (Position*)g_hash_table_lookup(component_registry.positions, entity_id);
    Vector2 coord = position->coord;
    SpriteRect sprite_rect = sprite_sheet[sprite->sprite_id];

    Vector2 global_coord = convert_local_to_global(coord.x, coord.y);
    float global_x = global_coord.x;
    float global_y = global_coord.y;

    Vector2 offset = sprite_rect.offset;
    offset = (Vector2){-sprite_rect.rect.width/2-offset.x, -offset.y};

    BeginMode2D(camera);

    DrawTextureRec(texture_sprite_sheet, sprite_sheet[SPRITE_SELECTED].rect, (Vector2){global_x+offset.x, global_y+offset.y}, WHITE);
    DrawTextureRec(texture_sprite_sheet, sprite_rect.rect, (Vector2){global_x+offset.x, global_y+offset.y}, WHITE);

    EndMode2D();
}

void render_device_mouseover_hover() {
    int screen_height = GetScreenHeight();
    Vector2 mouse_pos = GetMousePosition();
    Vector2 current_tile = convert_global_to_local(mouse_pos.x, mouse_pos.y);

    if (selectedDevice && mouse_pos.y > (screen_height - PANEL_HEIGHT)) {
        return;
    }

    Device* device = find_device_by_coord(current_tile.x, current_tile.y);
    if (device != NULL) {
        if (device->visible) {
            draw_device_id(*device, mouse_pos);
        } else {
            draw_label("???", mouse_pos);
        }
    }
}

void initialize_device_ui_system() {
    selectedDevice = NULL;
    deviceInfoWindowState = init_device_info_window(selectedDevice);
}

void update_device_ui_system() {
    if (update_device_info_window(&deviceInfoWindowState)) return;

    Vector2 mouse_pos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 clicked_tile = convert_global_to_local(mouse_pos.x, mouse_pos.y);
        Device* device = find_device_by_coord(clicked_tile.x, clicked_tile.y);

        if (device != NULL && device->visible) {
            selectedDevice = device;

            char buffer[50] = "";
            strcat(buffer, (device == NULL || device->type != DEVICE_TYPE_ROUTER) ? "#224#" : "#225#");
            deviceInfoWindowState.device = device;
            strcat(buffer, (device != NULL ? device->id : "Device"));
            strcpy(deviceInfoWindowState.windowTitle, buffer);
            deviceInfoWindowState.windowActive = true;
        }
    }
}

void render_device_ui_system() {
    render_selected_device();

    render_device_mouseover_hover();
    draw_mouse_coords();

    if (render_device_window(&deviceInfoWindowState)) {
        selectedDevice = NULL;
    }
}

