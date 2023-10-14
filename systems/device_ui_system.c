#include <stdio.h>
#include "glib.h"
#include "device_ui_system.h"
#include "utils/rendering.h"
#include "../components/component_registry.h"

Device* selected_device;

void draw_popover(int x, int y, char* message) {
    DrawRectangle(x-1, y-12, MeasureText(message, 10)+2, 10, BLACK);
    DrawText(message, x, y-12, 10, GREEN);
}

void draw_device_id(Device device, Vector2 coord) {
    int width = MeasureText(device.id, 10);
    DrawRectangle(coord.x-1, coord.y-11, width+2, 10, BLACK);
    DrawText(device.id, coord.x, coord.y-11, 10, GREEN);
}

void detect_mouse_collision() {
    Vector2 mouse_pos = GetMousePosition();
    Vector2 current_tile = convert_global_to_local(mouse_pos.x, mouse_pos.y);

    Device* device = find_device_by_coord(current_tile.x, current_tile.y);
    if (device != NULL) {
        device = find_device_by_coord(current_tile.x, current_tile.y);

        draw_device_id(*device, mouse_pos);
    }
}

void render_device_info() {
    int screen_height = GetScreenHeight();
    int screen_width = GetScreenWidth();

    int panel_width = screen_width;
    int panel_height = 200;

    int x = 0;
    int y = screen_height - panel_height;

    DrawRectangle(x, y, panel_width, panel_height, BLACK);
    DrawRectangleLinesEx((Rectangle){x, y, panel_width, panel_height}, 3, RAYWHITE);

    int top_pos = y+6;
    int left_pad = 6;
    char buffer[100] = "";
    sprintf(buffer, "Device ID: %s    Device Type: %s", selected_device->id, DeviceTypeLabel[selected_device->type]);
    DrawText(buffer, left_pad, top_pos, 20, BLUE);

    char* entity_id = selected_device->entity_id;
    Connection* conn = (Connection*)g_hash_table_lookup(component_registry.connections, entity_id);
    top_pos += 23;
    sprintf(buffer, "Connections (%d):", conn->num_conns);
    DrawText(buffer, left_pad, top_pos, 20, BLUE);
    for (int i = 0; i < conn->num_conns; i++) {
        conn->to_device_id[i];
    }
}

void render_selected_device() {
    if (selected_device == NULL) return;

    char* entity_id = selected_device->entity_id;
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

    render_device_info();
}

void initialize_device_ui_system() {
    selected_device = NULL;
}

void update_device_ui_system() {
    // Selection
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_pos = GetMousePosition();

        Vector2 clicked_tile = convert_global_to_local(mouse_pos.x, mouse_pos.y);

        Device* device = find_device_by_coord(clicked_tile.x, clicked_tile.y);

        if (device != NULL) {
            selected_device = device;
        }
    }
}

void render_device_ui_system() {
    render_selected_device();

    detect_mouse_collision();
    draw_mouse_coords();
}

