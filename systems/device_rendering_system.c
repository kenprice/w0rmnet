#include "device_rendering_system.h"
#include "../components/components.h"
#include "../world/world_map.h"
#include "../world/world_state.h"
#include "../graphics/tiles.h"
#include "utils/rendering.h"

void draw_area() {
    float iso_x = 0;
    float iso_y = 0;
    int iso_w = SPRITE_X_SCALE/2;
    int iso_h = SPRITE_Y_SCALE/2;

    int num_x_tiles = worldState.currentArea->width;
    int num_y_tiles = worldState.currentArea->height;

    for (int y = 0; y < num_y_tiles; y++) {
        for (int x = 0; x < num_x_tiles; x++) {
            int global_x = iso_x + (x - y) * iso_w;
            int global_y = iso_y + (x + y) * iso_h;

            Vector2 offset = tileSheet[0].offset;
            offset = (Vector2){-tileSheet[0].rect.width / 2 - offset.x, -offset.y};

            DrawTextureRec(textureTileSheet, tileSheet[0].rect, (Vector2){global_x + offset.x, global_y + offset.y}, WHITE);
        }
    }
}

void initialize_device_rendering_system() {
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    float map_offset = (float)(worldState.currentArea->width + worldState.currentArea->height) * (MAP_TILE_HEIGHT/2) / 2;

    initialize_camera((float)screen_width/2, (float)screen_height/2 - map_offset);
}

void update_device_rendering_system() {
    int offset = 10;

    if (IsKeyDown(KEY_D)) camera.offset.x -= offset;
    else if (IsKeyDown(KEY_A)) camera.offset.x += offset;

    if (IsKeyDown(KEY_W)) camera.offset.y += offset;
    else if (IsKeyDown(KEY_S)) camera.offset.y -= offset;
}

void draw_sprite(SpriteRect sprite_rect, Vector2 coord) {
    Vector2 global_coord = convert_local_to_global(coord.x, coord.y);
    float global_x = global_coord.x;
    float global_y = global_coord.y;

    Rectangle rect = sprite_rect.rect;
    Vector2 offset = sprite_rect.offset;
    offset = (Vector2){-rect.width/2-offset.x, -offset.y};
    Vector2 position = (Vector2){global_x+offset.x, global_y+offset.y};

    DrawTextureRec(textureSpriteSheet, rect, position, WHITE);
}

void render_packet(char* entity_id, PacketBuffer* packet_buffer) {
    Position* from_pos = (Position*)g_hash_table_lookup(componentRegistry.positions, entity_id);
    Vector2 global_coord = convert_local_to_global(from_pos->coord.x, from_pos->coord.y);

    // Render SEND messages
    PacketQueue q = packet_buffer->sendQ;
    int offset = 12;
    for (int i = q.tail; i < q.head; i++) {
        Packet* packet = q.packets[i];
        char message[100] = "";
        strcat(message, packet->message);
        strcat(message, " to ");
        strcat(message, packet->toAddress);
        DrawRectangle(global_coord.x-1, global_coord.y+offset, MeasureText(message, 10)+2, 10, BLACK);
        DrawText(message, global_coord.x, global_coord.y+offset, 10, GREEN);
        offset += 12;
    }
    // Render RECV messages
    q = packet_buffer->recvQ;
    for (int i = q.tail; i < q.head; i++) {
        Packet* packet = q.packets[i];
        char* message = packet->message;
        DrawRectangle(global_coord.x-1, global_coord.y+offset, MeasureText(message, 10)+2, 10, BLACK);
        DrawText(message, global_coord.x, global_coord.y+offset, 10, BLUE);
        offset += 12;
    }
}

void render_connection(char* entityId, Connection* connection) {
    char* fromEntity = entityId;

    char* toEntity = connection->parentEntityId;
    if (!toEntity || strlen(toEntity) == 0) return;

    Position* from_pos = (Position*)g_hash_table_lookup(componentRegistry.positions, fromEntity);
    Position* to_pos = (Position*)g_hash_table_lookup(componentRegistry.positions, toEntity);

    Vector2 from_coord = convert_local_to_global(from_pos->coord.x, from_pos->coord.y);
    from_coord.y += SPRITE_Y_SCALE/2;
    Vector2 to_coord = convert_local_to_global(to_pos->coord.x, to_pos->coord.y);
    to_coord.y += SPRITE_Y_SCALE/2;

    DrawLineEx(from_coord, to_coord, 3, WHITE);
}

void render_device_sprite(char* entity_id, Device* device) {
    Sprite* sprite = (Sprite*)g_hash_table_lookup(componentRegistry.sprites, entity_id);

    // Render Sprite
    Position* position = (Position*)g_hash_table_lookup(componentRegistry.positions, entity_id);

    if (!device->visible) {
        draw_sprite(spriteSheet[SPRITE_UNKNOWN], position->coord);
        return;
    } else {
        draw_sprite(spriteSheet[sprite->spriteId], position->coord);
    }

    if (device->owner != DEVICE_OWNER_PLAYER) {
        Vector2 global_coord = convert_local_to_global(position->coord.x, position->coord.y);
        float global_x = global_coord.x;
        float global_y = global_coord.y;
        DrawText("L", global_x, global_y, 10, BLUE);
    }
}

void render_device_rendering_system() {

    return;

    BeginMode2D(camera);
//    draw_isometric_grid();
    draw_area();

    Area* currentArea = worldState.currentArea;

    for (int i = 0; i < currentArea->numEntities; i++) {
        char* entity_id = currentArea->entities[i];
        Connection* conn = (Connection*)g_hash_table_lookup(componentRegistry.connections, entity_id);
        if (conn == NULL) continue;
        render_connection(entity_id, conn);
    }

    for (int i = 0; i < currentArea->numEntities; i++) {
        char* entity_id = currentArea->entities[i];
        Device* device = (Device*)g_hash_table_lookup(componentRegistry.devices, entity_id);
        if (device == NULL) continue;
        render_device_sprite(entity_id, device);
    }

    for (int i = 0; i < currentArea->numEntities; i++) {
        char* entity_id = currentArea->entities[i];
        PacketBuffer* pbuffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entity_id);
        if (pbuffer == NULL) continue;
        render_packet(entity_id, pbuffer);
    }
    EndMode2D();
}