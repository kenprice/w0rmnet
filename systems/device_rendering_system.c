#include "device_rendering_system.h"
#include "../components/components.h"
#include "utils/rendering.h"

/**
 * Isometric coord ("local") to "global" coord.
 * https://yal.cc/understanding-isometric-grids/
 */
Vector2 convert_local_to_global(float x, float y) {
    float iso_x = camera.offset.x;
    float iso_y = camera.offset.y;
    float iso_w = (float)SPRITE_X_SCALE/2;
    float iso_h = (float)SPRITE_Y_SCALE/2;
    float global_x = iso_x + (x - y) * iso_w;
    float global_y = iso_y + (x + y) * iso_h;
    return (Vector2){global_x, global_y};
}

Vector2 convert_global_to_local(float x, float y) {
    float iso_x = camera.offset.x;
    float iso_y = camera.offset.y;
    float iso_w = (float)SPRITE_X_SCALE/2;
    float iso_h = (float)SPRITE_Y_SCALE/2;
    float local_x = ((y - iso_y) / iso_h + (x - iso_x) / iso_w) / 2;
    float local_y = ((y - iso_y) / iso_h - (x - iso_x) / iso_w) / 2;
    return (Vector2){local_x, local_y};
}

void draw_mouse_coords() {
    Vector2 mouse_pos = GetMousePosition();
    char buffer[1000];
    sprintf(buffer, "Mouse Global: (%d,%d)", (int)mouse_pos.x, (int)mouse_pos.y);
    DrawText(buffer, 3, 3, 10, WHITE);

    Vector2 local_pos = convert_global_to_local(mouse_pos.x, mouse_pos.y);
    sprintf(buffer, "Mouse Local: (%d,%d)", (int)local_pos.x, (int)local_pos.y);
    DrawText(buffer, 3, 15, 10, WHITE);

    sprintf(buffer, "Camera Offset: (%d,%d)", (int)camera.offset.x, (int)camera.offset.y);
    DrawText(buffer, 3, 27, 10, WHITE);
}

void draw_isometric_grid_tile(float x, float y) {
    int iso_w = SPRITE_X_SCALE/2;
    int iso_h = SPRITE_Y_SCALE/2;
    DrawLine(x, y, x+iso_w, y+iso_h, WHITE);
    DrawLine(x, y, x-iso_w, y+iso_h, WHITE);
    DrawLine(x, y+(iso_h*2), x+iso_w, y+iso_h, WHITE);
    DrawLine(x, y+(iso_h*2), x-iso_w, y+iso_h, WHITE);
}

void draw_isometric_grid() {
    float iso_x = camera.offset.x;
    float iso_y = camera.offset.y;
    int iso_w = SPRITE_X_SCALE/2;
    int iso_h = SPRITE_Y_SCALE/2;

    int num_x_tiles = 12;
    int num_y_tiles = 12;

    for (int y = 0; y < num_y_tiles; y++) {
        for (int x = 0; x < num_x_tiles; x++) {
            int global_x = iso_x + (x - y) * iso_w;
            int global_y = iso_y + (x + y) * iso_h;
            draw_isometric_grid_tile(global_x, global_y);

            char buffer[100];
            sprintf(buffer, "%d,%d", x, y);
            DrawText(buffer, global_x, global_y+10, 10, BLACK);
        }
    }
}

void initialize_device_rendering_system() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    initialize_camera(screen_width, screen_height);
}

void update_device_rendering_system() {
    update_camera();
}

void draw_sprite(Texture2D texture, SpriteRect sprite_rect, Vector2 coord) {
    Vector2 global_coord = convert_local_to_global(coord.x, coord.y);
    float global_x = global_coord.x;
    float global_y = global_coord.y;

    Rectangle rect = sprite_rect.rect;
    Vector2 offset = sprite_rect.offset;
    offset = (Vector2){-rect.width/2-offset.x, -offset.y};
    Vector2 position = (Vector2){global_x+offset.x, global_y+offset.y};

    DrawTextureRec(texture, rect, position, WHITE);
}

void draw_device_id(Device device, Vector2 coord) {
    Vector2 global_coord = convert_local_to_global(coord.x, coord.y);
    int width = MeasureText(device.id, 10);
    DrawRectangle(global_coord.x-1, global_coord.y, width+2, 10, BLACK);
    DrawText(device.id, global_coord.x, global_coord.y, 10, GREEN);
}

void render_packets(ComponentRegistry* registry) {
    GHashTableIter iter;
    guint* key_;

    PacketBuffer* packet_buffer;
    g_hash_table_iter_init(&iter, registry->packet_buffers);
    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &packet_buffer)) {
        Position* from_pos = (Position*)g_hash_table_lookup(registry->positions, packet_buffer->entity_id);
        Vector2 global_coord = convert_local_to_global(from_pos->coord.x, from_pos->coord.y);

        // Render SEND packets
        Packet* packet = packet_buffer->send_q.packets[packet_buffer->send_q.tail];
        if (packet) {
            char* message = packet->message;
            DrawRectangle(global_coord.x-1, global_coord.y+12, MeasureText(message, 10)+2, 10, BLACK);
            DrawText(message, global_coord.x, global_coord.y+12, 10, GREEN);
        }

        // Render RECV packets
        packet = packet_buffer->recv_q.packets[packet_buffer->recv_q.tail];
        if (packet) {
            char* message = packet->message;
            DrawRectangle(global_coord.x-1, global_coord.y+12, MeasureText(message, 10)+2, 10, BLACK);
            DrawText(message, global_coord.x, global_coord.y+12, 10, BLUE);
        }
    }
}

void render_connection(ComponentRegistry* registry, Connection connection) {
    char* from_entity = find_device_entity_id_by_device_id(registry, connection.from_device_id);

    for (int i = 0; i < connection.num_conns; i++) {
        char* to_entity = find_device_entity_id_by_device_id(registry, connection.to_device_id[i]);

        Position* from_pos = (Position*)g_hash_table_lookup(registry->positions, from_entity);
        Position* to_pos = (Position*)g_hash_table_lookup(registry->positions, to_entity);

        Vector2 from_coord = convert_local_to_global(from_pos->coord.x, from_pos->coord.y);
        from_coord.y += SPRITE_Y_SCALE/2;
        Vector2 to_coord = convert_local_to_global(to_pos->coord.x, to_pos->coord.y);
        to_coord.y += SPRITE_Y_SCALE/2;

        DrawLineEx(from_coord, to_coord, 3, WHITE);
    }
}

void render_device_rendering_system(Texture2D texture, ComponentRegistry* registry) {
    draw_isometric_grid();

    GHashTableIter iter;
    guint* key_;

    Connection* connection;
    g_hash_table_iter_init(&iter, registry->connections);
    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &connection)) {
        render_connection(registry, *connection);
    }

    Sprite* sprite;
    g_hash_table_iter_init(&iter, registry->sprites);
    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &sprite)) {
        // Render Sprite
        Position* position = (Position*)g_hash_table_lookup(registry->positions, key_);
        draw_sprite(texture, sprite_sheet[sprite->sprite_id], position->coord);

        Device* dev = (Device*)g_hash_table_lookup(registry->devices, key_);
        draw_device_id(*dev, position->coord);
    }

    render_packets(registry);

    draw_mouse_coords();
}
