#include "machine.h"
#include "../components/component_registry.h"

/**
 * Create new "blank" machine. Sets entity ID and device ID, sets defaults.
 */
Machine* entity_machine_create_blank() {
    Machine* machine = calloc(1, sizeof(Machine));
    char* entity_id = generate_uuid();
    machine->entity_id = entity_id;

    char* device_id = generate_device_id();
    machine->device.type = DEVICE_TYPE_GENERIC;
    strncpy(machine->device.entity_id, entity_id, UUID_STR_LEN);
    strncpy(machine->device.id, device_id, DEVICE_ID_LEN);

    machine->position.coord.x = 0;
    machine->position.coord.y = 0;

    machine->sprite.sprite_id = SPRITE_SERVER;

    strncpy(machine->connection.from_device_id, device_id, DEVICE_ID_LEN);
    machine->connection.num_conns = 0;
    machine->connection.max_conns = 1;

    strncpy(machine->packet_buffer.entity_id, entity_id, UUID_STR_LEN);
    machine->packet_buffer.send_q = packet_queue_alloc(10);
    machine->packet_buffer.recv_q = packet_queue_alloc(10);

    return machine;
}

char* entity_machine_register_components(Machine machine) {
    char* entity_id = machine.entity_id;

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
    packet_buffer->send_q = packet_queue_alloc(10);
    packet_buffer->recv_q = packet_queue_alloc(10);
    strcpy(packet_buffer->entity_id, entity_id);
    g_hash_table_insert(component_registry.packet_buffers, entity_id, packet_buffer);

    return entity_id;
}
