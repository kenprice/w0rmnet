#include "machine.h"
#include "../components/component_registry.h"

char* create_machine_full(Machine machine) {
    char* entity_id = generate_uuid();

    register_device(machine.device, entity_id);

    Position* position = calloc(1, sizeof(Position));
    memcpy(position, &machine.position, sizeof(Position));
    g_hash_table_insert(component_registry.positions, entity_id, position);

    Sprite* sprite = calloc(1, sizeof(Sprite));
    memcpy(sprite, &machine.sprite, sizeof(Sprite));
    g_hash_table_insert(component_registry.sprites, entity_id, sprite);

    Connection* connection = calloc(1, sizeof(Connection)); // calloc initializes to zeroes
    memcpy(connection, &machine.connection, sizeof(Connection));
    g_hash_table_insert(component_registry.connections, entity_id, connection);

    PacketBuffer* packet_buffer = calloc(1, sizeof(PacketBuffer));
    packet_buffer->send_q = *packet_queue_alloc(10);
    packet_buffer->recv_q = *packet_queue_alloc(10);
    strcpy(packet_buffer->entity_id, entity_id);
    g_hash_table_insert(component_registry.packet_buffers, entity_id, packet_buffer);

    return entity_id;
}