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

void detect_mouse_collision() {
    Vector2 mouse_pos = GetMousePosition();
    Vector2 current_tile = convert_global_to_local(mouse_pos.x, mouse_pos.y);

    Device* device = find_device_by_coord(current_tile.x, current_tile.y);
    if (device != NULL) {
        if (device->visible) {
            draw_device_id(*device, mouse_pos);
        } else {
            draw_label("???", mouse_pos);
        }
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
    int text_width = 0;
    char buffer[100] = "";

    // Device ID: h3ll0w0rld
    sprintf(buffer, "Device ID: ");
    text_width = MeasureText(buffer, 20);
    DrawText(buffer, left_pad, top_pos, 20, WHITE);
    sprintf(buffer, "%s", selected_device->id);
    DrawText(buffer, left_pad+text_width, top_pos, 20, BLUE);
    top_pos += 23;

    sprintf(buffer, "Device Type: ");
    text_width = MeasureText(buffer, 20);
    DrawText(buffer, left_pad, top_pos, 20, WHITE);
    sprintf(buffer, "%s", DeviceTypeLabel[selected_device->type]);
    DrawText(buffer, left_pad+text_width, top_pos, 20, BLUE);
    top_pos += 23;

    bool player_owned = selected_device->owner == DEVICE_OWNER_PLAYER;
    sprintf(buffer, "Owner: ");
    text_width = MeasureText(buffer, 20);
    DrawText(buffer, left_pad, top_pos, 20, WHITE);
    sprintf(buffer, "%s", player_owned ? "You" : "???");
    DrawText(buffer, left_pad+text_width, top_pos, 20, player_owned ? GREEN : GRAY);
    top_pos += 23;

    char* entity_id = selected_device->entity_id;
    if (selected_device->type == DEVICE_TYPE_GENERIC) {
        Connection* conn = (Connection*)g_hash_table_lookup(component_registry.connections, entity_id);
        if (conn != NULL && strlen(conn->to_device_id[0]) > 0) {
            sprintf(buffer, "Network: ");
            text_width = MeasureText(buffer, 20);
            DrawText(buffer, left_pad, top_pos, 20, WHITE);
            sprintf(buffer, "%s", conn->to_device_id[0]);
            DrawText(buffer, left_pad+text_width, top_pos, 20, BLUE);
            top_pos += 23;
        }
    } else if (selected_device->type == DEVICE_TYPE_ROUTER) {
        RouteTable* route_table = (RouteTable*)g_hash_table_lookup(component_registry.route_tables, entity_id);
        if (route_table != NULL && strlen(route_table->gateway) > 0) {
            sprintf(buffer, "Network: ");
            text_width = MeasureText(buffer, 20);
            DrawText(buffer, left_pad, top_pos, 20, WHITE);
            sprintf(buffer, "%s", route_table->gateway);
            DrawText(buffer, left_pad+text_width, top_pos, 20, BLUE);
            top_pos += 23;
        }
    }

    // Next column
    top_pos = y+6;
    left_pad = panel_width/3;


    Connection* conn = (Connection*)g_hash_table_lookup(component_registry.connections, entity_id);
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

        if (device != NULL && device->visible) {
            selected_device = device;
        } else {
            selected_device = NULL;
        }
    }
}

void render_device_ui_system() {
    render_selected_device();

    detect_mouse_collision();
    draw_mouse_coords();
}

