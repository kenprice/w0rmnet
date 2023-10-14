#include "ping_runner.h"
#include "../../components/component_registry.h"

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

void proc_ping_handle_packet(char* entity_id, Process* process, Packet* packet) {
    if (packet == NULL) return;

    bool isValid = false;

    if (strcmp(packet->message, "Ping?") == 0) {
        bool respond = true;
        if ((int)process->state[REGION_RESTRICTED_MODE] == RESTRICTED_MODE_LOCAL) {
            int address_depth = count_address_depth(packet->to_address);
            respond = address_depth <= 2;
        }
        if (respond) {
            PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, entity_id);
            packet_queue_write(&packet_buffer->send_q, packet_alloc(entity_id, packet->from_address, "Pong!"));
        }
        isValid = true;
    }

    if (strcmp(packet->message, "Pong!") == 0) {
        isValid = true;
    }

    // Received packet is valid ping-pong packet, make sender visible if recipient is owned by player
    if (isValid) {
        Device* recipient_device = (Device*)g_hash_table_lookup(component_registry.devices, entity_id);
        if (recipient_device->owner == DEVICE_OWNER_PLAYER) {
            // Target entity is the one we want to determine the new visibility of
            char* target_entity = packet->from_entity_id;
            Device* target_device = (Device*)g_hash_table_lookup(component_registry.devices, target_entity);
            if (target_device && !target_device->visible) {
                target_device->visible = true;
            }
        }
    }
}

void proc_ping_handle_message(char* entity_id, Process* process, ProcMessage* message) {
    if (message == NULL) return;
    if (strlen(message->args) == 0) return;

    // Sends Ping to address specified by args
    char* address = message->args;
    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, entity_id);
    packet_queue_write(&packet_buffer->send_q, packet_alloc(entity_id, address, "Ping?"));
}
