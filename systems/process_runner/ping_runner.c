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
 * Counts the dots in an address, which kinda gives us the depth of the from-address
 * to determine if it's local to recipient.
 *
 * For example, we have three hosts X.A.a, X.A.b, X.B.a
 *  To A.a, A.b is local.
 *  To A.a, X.B.a is not
 *
 * @param address
 */
int count_address_depth(char* address) {
    char buffer[200];
    strcpy(buffer, address);
    char* token = strtok(buffer, ".");
    int parts = 0;
    while (token != NULL) {
        token = strtok(NULL, ".");
        parts++;
    }
    return parts;
}

void proc_ping_handle_packet(char* entityId, Process* process, Packet* packet) {
    if (packet == NULL) return;

    bool isValid = false;

    if (strcmp(packet->message, "Ping?") == 0) {
        bool respond = false;
        if ((int)process->state[REGION_RESTRICTED_MODE] == RESTRICTED_MODE_LOCAL) {
            // Must have entity in common in both route tables
            RouteTable* fromRt = g_hash_table_lookup(componentRegistry.routeTable, packet->fromEntityId);
            RouteTable* thisRt = g_hash_table_lookup(componentRegistry.routeTable, entityId);
            if (!fromRt || !thisRt) return;
            for (int i = 0; !respond && i < fromRt->numRecords; i++) {
                Wire* fromWire = g_hash_table_lookup(componentRegistry.wires, fromRt->records[i].wireEntityId);
                for (int j = 0; !respond && j < thisRt->numRecords; j++) {
                    Wire* thisWire = g_hash_table_lookup(componentRegistry.wires, thisRt->records[j].wireEntityId);

                    if (strcmp(fromWire->entityA, thisWire->entityA) == 0 ||
                        strcmp(fromWire->entityA, thisWire->entityB) == 0 ||
                        strcmp(fromWire->entityB, thisWire->entityA) == 0 ||
                        strcmp(fromWire->entityB, thisWire->entityB) == 0)
                    {
                        respond = true;
                    }
                }
            }
        } else {
            respond = true;
        }
        if (respond) {
            PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entityId);
            packet_queue_write(&packet_buffer->sendQ, packet_alloc(entityId, packet->toAddress, packet->fromAddress, "Pong!"));
        }
        isValid = true;
    }

    if (strcmp(packet->message, "Pong!") == 0) {
        isValid = true;
    }

    // Received packet is valid ping-pong packet, make sender visible if recipient is owned by player
    if (isValid) {
        Device* recipient_device = (Device*)g_hash_table_lookup(componentRegistry.devices, entityId);
        if (recipient_device->owner == DEVICE_OWNER_PLAYER) {
            // Target entity is the one we want to determine the new visibility of
            char* target_entity = packet->fromEntityId;
            Device* target_device = (Device*)g_hash_table_lookup(componentRegistry.devices, target_entity);
            if (target_device && !target_device->visible) {
                target_device->visible = true;
            }
        }
    }
}

void proc_ping_handle_message(char* entityId, Process* process, ProcMessage* message) {
    if (message == NULL) return;
    if (strlen(message->args) == 0) return;

    Device* fromDevice = g_hash_table_lookup(componentRegistry.devices, entityId);
    char* fromAddress = fromDevice->address;

    // Sends Ping to address specified by args
    char* targetAddress = message->args;

//    char* relativeTargetAddress = convert_full_address_to_relative_address(entityId, targetAddress);


    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entityId);
    packet_queue_write(&packet_buffer->sendQ, packet_alloc(entityId, fromAddress, targetAddress, "Ping?"));

//    free(relativeTargetAddress);
}
