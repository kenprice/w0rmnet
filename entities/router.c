#include "router.h"

char* create_router_full(ComponentRegistry* registry, Router router) {
    char* entity_id = generate_uuid();

    register_device(registry, router.device, entity_id);

    Position* position = calloc(1, sizeof(Position));
    memcpy(position, &router.position, sizeof(Position));
    g_hash_table_insert(registry->positions, entity_id, position);

    Sprite* sprite = calloc(1, sizeof(Sprite));
    memcpy(sprite, &router.sprite, sizeof(Sprite));
    g_hash_table_insert(registry->sprites, entity_id, sprite);

    Connection* connection = calloc(1, sizeof(Connection)); // calloc initializes to zeroes
    memcpy(connection, &router.connection, sizeof(Connection));
    g_hash_table_insert(registry->connections, entity_id, connection);

    PacketBuffer* packet_buffer = calloc(1, sizeof(PacketBuffer));
    packet_buffer->send_q = *packet_queue_alloc(10);
    packet_buffer->recv_q = *packet_queue_alloc(10);
    strcpy(packet_buffer->entity_id, entity_id);
    g_hash_table_insert(registry->packet_buffers, entity_id, packet_buffer);

    return entity_id;
}
