#include "machine.h"
#include "../components/component_registry.h"

/**
 * Create new "blank" machine. Sets entity ID and device ID, sets defaults.
 */
Machine* entity_machine_create_blank() {
    Machine* machine = calloc(1, sizeof(Machine));
    char* entity_id = generate_uuid();
    machine->entityId = entity_id;

    char* device_id = generate_device_id();
    machine->device.type = DEVICE_TYPE_GENERIC;
    machine->device.owner = DEVICE_OWNER_NOBODY;
    machine->device.pwned = DEVICE_NOT_PWNED;
    strncpy(machine->device.entityId, entity_id, UUID_STR_LEN);
    strncpy(machine->device.name, device_id, DEVICE_NAME_LEN);

    machine->position.coord.x = 0;
    machine->position.coord.y = 0;

    machine->sprite.spriteId = SPRITE_SERVER;

    machine->connection.numConns = 0;
    machine->connection.maxConns = 1;

    strncpy(machine->packetBuffer.entityId, entity_id, UUID_STR_LEN);
    machine->packetBuffer.sendQ = packet_queue_alloc(10);
    machine->packetBuffer.recvQ = packet_queue_alloc(10);

    return machine;
}

char* entity_machine_register_components(Machine machine) {
    char* entity_id = machine.entityId;

    register_device(machine.device, entity_id);

    Position* position = calloc(1, sizeof(Position));
    memcpy(position, &machine.position, sizeof(Position));
    g_hash_table_insert(componentRegistry.positions, entity_id, position);

    Sprite* sprite = calloc(1, sizeof(Sprite));
    memcpy(sprite, &machine.sprite, sizeof(Sprite));
    g_hash_table_insert(componentRegistry.sprites, entity_id, sprite);

    Connection* connection = calloc(1, sizeof(Connection)); // calloc initializes to zeroes
    memcpy(connection, &machine.connection, sizeof(Connection));
    g_hash_table_insert(componentRegistry.connections, entity_id, connection);

    PacketBuffer* packet_buffer = calloc(1, sizeof(PacketBuffer));
    packet_buffer->sendQ = packet_queue_alloc(10);
    packet_buffer->recvQ = packet_queue_alloc(10);
    strcpy(packet_buffer->entityId, entity_id);
    g_hash_table_insert(componentRegistry.packetBuffers, entity_id, packet_buffer);

    return entity_id;
}
