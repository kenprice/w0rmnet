#include "glib.h"
#include "login_runner.h"
#include "../../components/component_registry.h"
#include "../utils/routing.h"

/**
 * Login State
 * N Bytes: "<username>:<password>"
 */

void proc_login_handle_packet(char* entity_id, Process* process, Packet* packet) {
    if (packet == NULL || packet->message == NULL) return;

    char** packetParts = g_strsplit(packet->message, " ", 2);
    char* command = packetParts[0];

    if (strcmp(command, "LOGIN") != 0) return;

    char* creds = packetParts[1];

    if (strcmp(creds, process->state) == 0) {
        Device* device = (Device*)g_hash_table_lookup(componentRegistry.devices, entity_id);
        device->owner = DEVICE_OWNER_PLAYER;
    }
}

void proc_login_handle_message(char* entity_id, Process* process, ProcMessage* message) {
    if (message == NULL) return;
    if (strlen(message->args) == 0) return;

    char packetMessage[100] = "LOGIN ";
    char** args = g_strsplit(message->args, ":", 2);

    char* address = args[0];
    char* creds = args[1]; // username:password
    strcat(packetMessage, creds);

    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entity_id);
    packet_queue_write(&packet_buffer->sendQ, packet_alloc(entity_id, address, packetMessage));
}
