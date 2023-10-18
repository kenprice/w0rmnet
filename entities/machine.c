#include <stdio.h>
#include <string.h>
#include "machine.h"
#include "../components/component_registry.h"
#include "../components/components.h"

/**
 * Create new "blank" machine. Sets entity ID and device ID, sets defaults.
 */
Machine* entity_machine_create_blank() {
    Machine* machine = calloc(1, sizeof(Machine));
    char* entity_id = generate_uuid();
    strcpy(machine->entityId, entity_id);

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
    char* entityId = strdup(machine.entityId);

    register_device(machine.device, entityId);

    Position* position = calloc(1, sizeof(Position));
    memcpy(position, &machine.position, sizeof(Position));
    g_hash_table_insert(componentRegistry.positions, entityId, position);

    Sprite* sprite = calloc(1, sizeof(Sprite));
    memcpy(sprite, &machine.sprite, sizeof(Sprite));
    g_hash_table_insert(componentRegistry.sprites, entityId, sprite);

    Connection* connection = calloc(1, sizeof(Connection)); // calloc initializes to zeroes
    memcpy(connection, &machine.connection, sizeof(Connection));
    g_hash_table_insert(componentRegistry.connections, entityId, connection);

    PacketBuffer* packetBuffer = calloc(1, sizeof(PacketBuffer));
    packetBuffer->sendQ = packet_queue_alloc(10);
    packetBuffer->recvQ = packet_queue_alloc(10);
    strcpy(packetBuffer->entityId, entityId);
    g_hash_table_insert(componentRegistry.packetBuffers, entityId, packetBuffer);

    ProcessManager* processManager = calloc(1, sizeof(ProcessManager));
    processManager->numProcs = machine.processManager.numProcs;
    processManager->maxProcs = machine.processManager.maxProcs;
    for (int i = 0; i < machine.processManager.numProcs; i++) {
        processManager->processes[i].type = machine.processManager.processes[i].type;
        processManager->processes[i].invocable = machine.processManager.processes[i].invocable;
        strcpy(processManager->processes[i].state, machine.processManager.processes[i].state);
    }
    g_hash_table_insert(componentRegistry.processManagers, entityId, processManager);

    register_proc_msg_queue(proc_msg_queue_alloc(10), entityId);

    return entityId;
}

/**
 * Tab-separated values of serialized components.
 * Serialized components themselves are semicolon-separated values
 *
 * @param machine
 * @return
 */
char* entity_machine_serialize(Machine machine) {
    char buffer[10000] = "";

    char* strDevice = comp_device_serialize(&machine.device);
    char* strPosition = comp_position_serialize(&machine.position);
    char* strSprite = comp_sprite_serialize(&machine.sprite);
    char* strConnection = comp_connection_serialize(&machine.connection);
    char* strPacketBuffer = comp_packet_buffer_serialize(&machine.packetBuffer);
    char* strProcessManager = comp_process_manager_serialize(&machine.processManager);

    sprintf(buffer, "%s\t%s\t%s\t%s\t%s\t%s\t%s", machine.entityId, strDevice, strPosition, strSprite, strConnection,
            strPacketBuffer, strProcessManager);

    char* data = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(data, buffer);

    free(strDevice);
    free(strPosition);
    free(strSprite);
    free(strConnection);
    free(strPacketBuffer);
    free(strProcessManager);

    return data;
}

Machine entity_machine_deserialize(char* data) {
    char** machineData = g_strsplit(data, "\t", 7);

    Machine machine;
    strcpy(machine.entityId, machineData[0]);
    machine.device = *(comp_device_deserialize(machineData[1]));
    machine.position = *(comp_position_deserialize(machineData[2]));
    machine.sprite = *(comp_sprite_deserialize(machineData[3]));
    machine.connection = *(comp_connection_deserialize(machineData[4]));
    machine.packetBuffer = *(comp_packet_buffer_deserialize(machineData[5]));
    machine.processManager = *(comp_process_manager_deserialize(machineData[6]));

    return machine;
}
