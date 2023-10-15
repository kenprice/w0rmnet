#include <stdio.h>
#include "glib.h"
#include "device_ui_system.h"
#include "utils/rendering.h"
#include "../components/component_registry.h"

#define PANEL_HEIGHT 200

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

void render_device_info() {
    int screen_height = GetScreenHeight();
    int screen_width = GetScreenWidth();

    int panel_width = screen_width;

    int x = 0;
    int y = screen_height - PANEL_HEIGHT;

    DrawRectangle(x, y, panel_width, PANEL_HEIGHT, BLACK);
    DrawRectangleLinesEx((Rectangle){x, y, panel_width, PANEL_HEIGHT}, 3, RAYWHITE);

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
        }
    }

    // Action pad right hand side
    top_pos = y+10;
    left_pad = screen_width - 190;

    for (int i=0; i<9; i++) {
        int x_offset = (i%3)*60+3;
        int y_offset = (i/3)*60+3;

        ProcessManager* processManager = (ProcessManager *)g_hash_table_lookup(component_registry.process_managers, entity_id);
        bool exists = processManager ? (i < processManager->num_procs) : false;

        DrawRectangleLinesEx((Rectangle){left_pad+x_offset, top_pos+y_offset, 54, 54}, 2, exists ? RAYWHITE : GRAY);
        if (exists) {
            // TODO: icons
            DrawText(ProcessTypeLabel[processManager->processes[i].type], left_pad+x_offset+6, top_pos+y_offset+6, 10, WHITE);
        }
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

void panel_mouse_collision() {

}

void render_device_mouseover_hover() {
    int screen_height = GetScreenHeight();
    Vector2 mouse_pos = GetMousePosition();
    Vector2 current_tile = convert_global_to_local(mouse_pos.x, mouse_pos.y);

    if (selected_device && mouse_pos.y > (screen_height-PANEL_HEIGHT)) {
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
    selected_device = NULL;
}

void update_device_ui_system() {
    int screen_height = GetScreenHeight();
    Vector2 mouse_pos = GetMousePosition();

    if (selected_device && mouse_pos.y > (screen_height-PANEL_HEIGHT)) {
        panel_mouse_collision();
        return;
    }
    // Selection

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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

    render_device_mouseover_hover();
    draw_mouse_coords();
}

