#include "device_rendering_system.h"
#include "../components/components.h"
#include "../world/world_map.h"
#include "utils/rendering.h"

void draw_isometric_grid_tile(float x, float y) {
    int iso_w = SPRITE_X_SCALE/2;
    int iso_h = SPRITE_Y_SCALE/2;
    DrawLine(x, y, x+iso_w, y+iso_h, WHITE);
    DrawLine(x, y, x-iso_w, y+iso_h, WHITE);
    DrawLine(x, y+(iso_h*2), x+iso_w, y+iso_h, WHITE);
    DrawLine(x, y+(iso_h*2), x-iso_w, y+iso_h, WHITE);
}

void draw_isometric_grid() {
    float iso_x = 0;
    float iso_y = 0;
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

    float map_offset = (float)(area_registry[area_current].width + area_registry[area_current].height) * (MAP_TILE_HEIGHT) / 2;

    initialize_camera((float)screen_width/2, (float)screen_height/2 - map_offset);
}

void update_device_rendering_system() {
    update_camera();
}

void draw_sprite(SpriteRect sprite_rect, Vector2 coord) {
    Vector2 global_coord = convert_local_to_global(coord.x, coord.y);
    float global_x = global_coord.x;
    float global_y = global_coord.y;

    Rectangle rect = sprite_rect.rect;
    Vector2 offset = sprite_rect.offset;
    offset = (Vector2){-rect.width/2-offset.x, -offset.y};
    Vector2 position = (Vector2){global_x+offset.x, global_y+offset.y};

    DrawTextureRec(texture_sprite_sheet, rect, position, WHITE);
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
    draw_sprite(sprite_sheet[sprite->sprite_id], position->coord);
}

void render_device_rendering_system() {
    BeginMode2D(camera);
    draw_isometric_grid();

    Area current_area = area_registry[area_current];

    for (int i = 0; i < current_area.num_entities; i++) {
        char* entity_id = current_area.entities[i];
        Connection* conn = (Connection*)g_hash_table_lookup(component_registry.connections, entity_id);
        if (conn == NULL) continue;
        render_connection(entity_id, conn);
    }

    for (int i = 0; i < current_area.num_entities; i++) {
        char* entity_id = current_area.entities[i];
        Device* device = (Device*)g_hash_table_lookup(component_registry.devices, entity_id);
        if (device == NULL) continue;
        render_device_sprite(entity_id, device);
    }

    for (int i = 0; i < current_area.num_entities; i++) {
        char* entity_id = current_area.entities[i];
        PacketBuffer* pbuffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, entity_id);
        if (pbuffer == NULL) continue;
        render_packet(entity_id, pbuffer);
    }
    EndMode2D();
}