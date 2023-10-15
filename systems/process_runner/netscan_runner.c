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
 * @param entity_id entity on which the process is running
 * @param process the process
 * @param message received message
 */
void proc_netscan_handle_message(char* entity_id, Process* process, ProcMessage* message) {
    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, entity_id);
    if (!packet_buffer) return;

    // Message contains address of target router
    char* target_router_address = message->args;
    char* found_entity_id = search_device_by_address(entity_id, target_router_address);
    if (!found_entity_id) return;

    // Perform a ping scan against target router's connections
    Connection* connection = (Connection*)g_hash_table_lookup(component_registry.connections, found_entity_id);
    for (int i = 0; i < connection->num_conns; i++) {
        if (strcmp(connection->to_device_id[i], connection->parent_device_id) == 0) continue;

        char to_address[110];
        strcpy(to_address, target_router_address);
        strcat(to_address, ".");
        strcat(to_address, connection->to_device_id[i]);
        packet_queue_write(&packet_buffer->send_q, packet_alloc(entity_id, to_address, "Ping?"));
    }
}
