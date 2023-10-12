#include "device_rendering_system.h"
#include "../components/components.h"
#include "utils/rendering.h"

Texture2D texture;

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

void initialize_device_rendering_system(Texture2D loaded_texture) {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    initialize_camera(screen_width, screen_height);

    texture = loaded_texture;
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

void draw_popover(int x, int y, char* message) {
    DrawRectangle(x-1, y-12, MeasureText(message, 10)+2, 10, BLACK);
    DrawText(message, x, y-12, 10, GREEN);
}

void render_packet(char* entity_id, PacketBuffer* packet_buffer) {
    Position* from_pos = (Position*)g_hash_table_lookup(component_registry.positions, entity_id);
    Vector2 global_coord = convert_local_to_global(from_pos->coord.x, from_pos->coord.y);

    // Render SEND packets
    PacketQueue q = packet_buffer->send_q;
    int offset = 12;
    for (int i = q.tail; i < q.head; i++) {
        Packet* packet = q.packets[i];
        char message[100] = "";
        strcat(message, packet->message);
        strcat(message, " to ");
        strcat(message, packet->to_address);
        DrawRectangle(global_coord.x-1, global_coord.y+offset, MeasureText(message, 10)+2, 10, BLACK);
        DrawText(message, global_coord.x, global_coord.y+offset, 10, GREEN);
        offset += 12;
    }
    // Render RECV packets
    q = packet_buffer->recv_q;
    for (int i = q.tail; i < q.head; i++) {
        Packet* packet = q.packets[i];
        char* message = packet->message;
        DrawRectangle(global_coord.x-1, global_coord.y+offset, MeasureText(message, 10)+2, 10, BLACK);
        DrawText(message, global_coord.x, global_coord.y+offset, 10, BLUE);
        offset += 12;
    }
}

void render_connection(char* _entity_id, Connection* connection) {
    char* from_entity = _entity_id;

    for (int i = 0; i < connection->num_conns; i++) {
        char* to_entity = find_device_entity_id_by_device_id(connection->to_device_id[i]);

        Position* from_pos = (Position*)g_hash_table_lookup(component_registry.positions, from_entity);
        Position* to_pos = (Position*)g_hash_table_lookup(component_registry.positions, to_entity);

        Vector2 from_coord = convert_local_to_global(from_pos->coord.x, from_pos->coord.y);
        from_coord.y += SPRITE_Y_SCALE/2;
        Vector2 to_coord = convert_local_to_global(to_pos->coord.x, to_pos->coord.y);
        to_coord.y += SPRITE_Y_SCALE/2;

        DrawLineEx(from_coord, to_coord, 3, WHITE);
    }

    RouteTable* route_table = (RouteTable*)g_hash_table_lookup(component_registry.route_tables, from_entity);
    if (route_table != NULL && strlen(route_table->gateway) > 0) {
        char* to_entity = find_device_entity_id_by_device_id(route_table->gateway);
        if (to_entity != NULL) {
            Position* to_pos = (Position*)g_hash_table_lookup(component_registry.positions, to_entity);
            Vector2 to_coord = convert_local_to_global(to_pos->coord.x, to_pos->coord.y);

//            DrawCircle(to_coord.x, to_coord.y, 100, BLUE);
        }
    }
}

void render_device_sprite(char* entity_id, Device* device) {
    Sprite* sprite = (Sprite*)g_hash_table_lookup(component_registry.sprites, entity_id);

    // Render Sprite
    Position* position = (Position*)g_hash_table_lookup(component_registry.positions, entity_id);
    draw_sprite(texture, sprite_sheet[sprite->sprite_id], position->coord);

    draw_device_id(*device, position->coord);
}

void detect_mouse_collision() {
    Vector2 mouse_pos = GetMousePosition();
    Vector2 current_tile = convert_global_to_local(mouse_pos.x, mouse_pos.y);

    Device* device = find_device_by_coord(current_tile.x, current_tile.y);
    if (device != NULL) {
        device = find_device_by_coord(current_tile.x, current_tile.y);
        char message[100];
        sprintf(message, "test");
        draw_popover(mouse_pos.x, mouse_pos.y, message);
    }

//    mouse_pos.x;
}

void render_device_rendering_system() {
    draw_isometric_grid();

    iterate_connections(render_connection);
    iterate_devices(render_device_sprite);
    iterate_packet_buffers(render_packet);
    detect_mouse_collision();

    draw_mouse_coords();
}
