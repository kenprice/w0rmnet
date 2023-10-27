#include <stdio.h>
#include "ping_runner.h"
#include "../utils/routing.h"
#include "../../components/component_registry.h"

/**
 * Ping State
 * 1st byte: Restricted Mode (default 0)
 */
#define REGION_RESTRICTED_MODE 0
#define RESTRICTED_MODE_UNRESTRICTED 0
#define RESTRICTED_MODE_LOCAL 1

/**
 * Do the two RouteTable-having entities have the same entity in common?
 * i.e. Do two machines have the same router in common? Are two devices adjacent? etc.
 */
static bool has_common_connected_wires(char* fromEntityId, char* toEntityId) {
    // Must have entity in common in both route tables
    RouteTable* fromRt = g_hash_table_lookup(componentRegistry.routeTable, fromEntityId);
    RouteTable* thisRt = g_hash_table_lookup(componentRegistry.routeTable, toEntityId);
    if (!fromRt || !thisRt) return false;
    for (int i = 0; i < fromRt->numRecords; i++) {
        Wire* fromWire = g_hash_table_lookup(componentRegistry.wires, fromRt->records[i].wireEntityId);
        for (int j = 0; j < thisRt->numRecords; j++) {
            Wire* thisWire = g_hash_table_lookup(componentRegistry.wires, thisRt->records[j].wireEntityId);
            bool found = (strcmp(fromWire->entityA, thisWire->entityA) == 0 ||
                          strcmp(fromWire->entityA, thisWire->entityB) == 0 ||
                          strcmp(fromWire->entityB, thisWire->entityA) == 0 ||
                          strcmp(fromWire->entityB, thisWire->entityB) == 0);
            if (found) {
                return true;
            }
        }
    }
    return false;
}

void proc_ping_handle_packet(char* entityId, Process* process, Packet* packet) {
    if (packet == NULL) return;

    Logger* logger = g_hash_table_lookup(componentRegistry.logger, entityId);
    char buffer[200];

    if (strcmp(packet->message, "Ping?") == 0) {
        bool isRestricted = (int)process->state[REGION_RESTRICTED_MODE] == RESTRICTED_MODE_LOCAL;
        bool respond = true;
        if (isRestricted) {
            respond = has_common_connected_wires(packet->fromEntityId, entityId);
        }
        if (respond) {
            PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entityId);
            packet_queue_write(&packet_buffer->sendQ, packet_alloc(entityId, packet->toAddress, packet->fromAddress, "Pong!"));
            sprintf(buffer, "Received PING from %s", packet->fromAddress);
            sprintf(buffer, "Sending PONG to %s", packet->fromAddress);
            comp_logger_add_entry(logger, buffer);
        } else {
            sprintf(buffer, "Blocked PING from %s", packet->fromAddress);
            comp_logger_add_entry(logger, buffer);
        }
    }

    if (strcmp(packet->message, "Pong!") == 0) {
        // Received packet is valid pong packet, make sender visible if recipient is owned by player
        bool newHost = false;
        Device* recipientDevice = (Device*)g_hash_table_lookup(componentRegistry.devices, entityId);
        if (recipientDevice->owner == DEVICE_OWNER_PLAYER) {
            // Target entity is the one we want to determine the new visibility of
            char* target_entity = packet->fromEntityId;
            Device* targetDevice = (Device*)g_hash_table_lookup(componentRegistry.devices, target_entity);
            if (targetDevice && !targetDevice->visible) {
                targetDevice->visible = true;
                newHost = true;
            }
        }

        sprintf(buffer, "Received PONG from %s", packet->fromAddress);
        comp_logger_add_entry(logger, buffer);
        if (newHost) {
            sprintf(buffer, "Host %s is new and has been added to known hosts", packet->fromAddress);
            comp_logger_add_entry(logger, buffer);
        }
    }
}

void proc_ping_handle_message(char* entityId, Process* process, ProcMessage* message) {
    if (message == NULL) return;
    if (strlen(message->args) == 0) return;

    Logger* logger = g_hash_table_lookup(componentRegistry.logger, entityId);
    char buffer[200];

    Device* fromDevice = g_hash_table_lookup(componentRegistry.devices, entityId);
    char* fromAddress = fromDevice->address;

    // Sends Ping to address specified by args
    char* targetAddress = message->args;

    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entityId);
    packet_queue_write(&packet_buffer->sendQ, packet_alloc(entityId, fromAddress, targetAddress, "Ping?"));

    sprintf(buffer, "PING %s", targetAddress);
    comp_logger_add_entry(logger, buffer);
}
