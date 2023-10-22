#include "netscan_runner.h"
#include "../../components/component_registry.h"
#include "../utils/routing.h"

/**
 * Netscan
 * -------
 *  Input: address of router
 * Effect: given a router, pings its connections
 */

/**
 * @param entity_id entity on which the process is running
 * @param process the process
 * @param packet received packet
 */
void proc_netscan_handle_packet(char* entity_id, Process* process, Packet* packet) {
    /* no-op */
}

/**
 * @param entityId entity on which the process is running
 * @param process the process
 * @param message received message
 */
void proc_netscan_handle_message(char* entityId, Process* process, ProcMessage* message) {
    PacketBuffer* packetBuffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entityId);
    if (!packetBuffer) return;

    // Message contains address of target router
    char* targetRouterAddress = message->args;
    Device* targetDevice = find_device_by_address(targetRouterAddress);
    if (!targetDevice) return;

    char* relativeTargetAddress = convert_full_address_to_relative_address(entityId, targetRouterAddress);

    // Perform a ping scan against target router's connections
    Connection* connection = (Connection*)g_hash_table_lookup(componentRegistry.connections, targetDevice->entityId);
    for (int i = 0; i < connection->numConns; i++) {
        if (strcmp(connection->toEntityIds[i], connection->parentEntityId) == 0) continue;

        Device* device = g_hash_table_lookup(componentRegistry.devices, connection->toEntityIds[i]);

        char toAddress[110];
        strcpy(toAddress, relativeTargetAddress);
        strcat(toAddress, ".");
        strcat(toAddress, device->name);
        packet_queue_write(&packetBuffer->sendQ, packet_alloc(entityId, toAddress, "Ping?"));
    }
    free(relativeTargetAddress);
}
