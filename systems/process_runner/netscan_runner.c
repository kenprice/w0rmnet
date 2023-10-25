#include "netscan_runner.h"
#include "../../components/component_registry.h"
#include "../utils/routing.h"
#include "../../components/wire.h"

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

    Device* fromDevice = g_hash_table_lookup(componentRegistry.devices, entityId);
    char* fromAddress = fromDevice->address;

    // Message contains address of target router
    char* targetAddress = message->args;
    Device* targetDevice = find_device_by_address(targetAddress);
    if (!targetDevice) return;

    // Perform a ping scan against target router's connections
    RouteTable* routeTable = g_hash_table_lookup(componentRegistry.routeTable, targetDevice->entityId);
    for (int i = 0; i < routeTable->numRecords; i++) {
        if (strcmp(routeTable->records[i].prefix, "*") == 0) continue;

        packet_queue_write(&packetBuffer->sendQ, packet_alloc(entityId, fromAddress, routeTable->records[i].prefix, "Ping?"));
    }
}
