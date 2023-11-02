#include <stdio.h>
#include "netscan_runner.h"
#include "../../components/component_registry.h"

/**
 * Netscan
 * -------
 *  Input: address of router
 * Effect: given a router, pings its connections
 */

/**
 * @param entityId entity on which the process is running
 * @param process the process
 * @param packet received packet
 */
void proc_netscan_handle_packet(char* entityId, Process* process, Packet* packet) {
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

    Logger* logger = g_hash_table_lookup(componentRegistry.logger, entityId);
    char buffer[200];

    Device* fromDevice = g_hash_table_lookup(componentRegistry.devices, entityId);
    char* fromAddress = fromDevice->address;

    // Message contains address of target router
    char* targetAddress = message->args;
    Device* targetDevice = find_device_by_address(targetAddress);
    if (!targetDevice) return;

    sprintf(buffer, "NETSCAN %s", targetAddress);
    comp_logger_add_entry(logger, buffer);

    // Perform a ping scan against target router's connections
    RouteTable* routeTable = g_hash_table_lookup(componentRegistry.routeTable, targetDevice->entityId);
    for (int i = 0; i < routeTable->numRecords; i++) {
        if (strcmp(routeTable->records[i].prefix, "*") == 0) continue;
        if (strcmp(routeTable->records[i].prefix, fromAddress) == 0) continue;

        char* otherDeviceId = comp_wire_get_other_entity(routeTable->records[i].wireEntityId, targetDevice->entityId);
        if (!otherDeviceId) continue;
        Device* otherDevice = g_hash_table_lookup(componentRegistry.devices, otherDeviceId);
        if (!otherDevice) continue;
        if (otherDevice->type == DEVICE_TYPE_ROUTER || otherDevice->type == DEVICE_TYPE_SWITCH) continue;

        packet_queue_write(&packetBuffer->sendQ, packet_alloc(entityId, fromAddress, routeTable->records[i].prefix, "Ping?"));
    }
}
