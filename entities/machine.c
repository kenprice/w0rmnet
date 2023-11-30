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

    machine->sprite.spriteId = SPRITE_LAPTOP;

    strncpy(machine->packetBuffer.entityId, entity_id, UUID_STR_LEN);
    machine->packetBuffer.sendQ = packet_queue_alloc(50);
    machine->packetBuffer.recvQ = packet_queue_alloc(50);

    return machine;
}

char* entity_machine_register_components(Machine* machine) {
    char* entityId = machine->entityId;

    g_hash_table_insert(componentRegistry.devices, entityId, &machine->device);
    g_hash_table_insert(componentRegistry.positions, entityId, &machine->position);
    g_hash_table_insert(componentRegistry.sprites, entityId, &machine->sprite);
    g_hash_table_insert(componentRegistry.routeTable, entityId, &machine->routeTable);
    g_hash_table_insert(componentRegistry.knownHosts, entityId, &machine->knownHosts);

    PacketBuffer* packetBuffer = calloc(1, sizeof(PacketBuffer));
    packetBuffer->sendQ = packet_queue_alloc(50);
    packetBuffer->recvQ = packet_queue_alloc(50);
    strcpy(packetBuffer->entityId, entityId);
    g_hash_table_insert(componentRegistry.packetBuffers, entityId, packetBuffer);

    g_hash_table_insert(componentRegistry.processManagers, entityId, &machine->processManager);
    register_proc_msg_queue(proc_msg_queue_alloc(10), entityId);

    Logger* logger = calloc(1, sizeof(Logger));
    logger->numEntries = 0;
    g_hash_table_insert(componentRegistry.logger, entityId, logger);

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
    char* strRouteTable = comp_route_table_serialize(&machine.routeTable);
    char* strPacketBuffer = comp_packet_buffer_serialize(&machine.packetBuffer);
    char* strProcessManager = comp_process_manager_serialize(&machine.processManager);

    sprintf(buffer, "%s\t%s\t%s\t%s\t%s\t%s\t%s", machine.entityId, strDevice, strPosition, strSprite, strRouteTable,
            strPacketBuffer, strProcessManager);

    char* data = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(data, buffer);

    free(strDevice);
    free(strPosition);
    free(strSprite);
    free(strRouteTable);
    free(strPacketBuffer);
    free(strProcessManager);

    return data;
}

Machine entity_machine_deserialize(char* data) {
    char** machineData = g_strsplit(data, "\t", 8);

    Machine machine;
    strcpy(machine.entityId, machineData[0]);
    machine.device = *(comp_device_deserialize(machineData[1]));
    machine.position = *(comp_position_deserialize(machineData[2]));
    machine.sprite = *(comp_sprite_deserialize(machineData[3]));
    machine.routeTable = *(comp_route_table_deserialize(machineData[4]));
    machine.packetBuffer = *(comp_packet_buffer_deserialize(machineData[5]));
    machine.processManager = *(comp_process_manager_deserialize(machineData[6]));

    return machine;
}
