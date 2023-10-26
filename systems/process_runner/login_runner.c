#include <stdio.h>
#include "glib.h"
#include "login_runner.h"
#include "../../components/component_registry.h"

/**
 * Login State
 * N Bytes: "<username>:<password>"
 */
void proc_login_handle_packet(char* entityId, Process* process, Packet* packet) {
    if (packet == NULL || packet->message == NULL) return;

    Logger* logger = g_hash_table_lookup(componentRegistry.logger, entityId);
    char buffer[200];

    char** packetParts = g_strsplit(packet->message, " ", 2);
    char* command = packetParts[0];

    if (strcmp(command, "LOGIN") != 0) return;

    char* creds = packetParts[1];

    if (strcmp(creds, process->state) == 0) {
        Device* device = (Device*)g_hash_table_lookup(componentRegistry.devices, entityId);
        device->owner = DEVICE_OWNER_PLAYER;

        sprintf(buffer, "Received LOGIN from %s. Login successful.", packet->fromAddress);
        comp_logger_add_entry(logger, buffer);
    } else {
        sprintf(buffer, "Received LOGIN from %s. Login failed.", packet->fromAddress);
        comp_logger_add_entry(logger, buffer);
    }

    g_strfreev(packetParts);
}

void proc_login_handle_message(char* entityId, Process* process, ProcMessage* message) {
    if (message == NULL) return;
    if (strlen(message->args) == 0) return;

    char packetMessage[100] = "LOGIN ";
    char** args = g_strsplit(message->args, ":", 2);

    char* toAddress =  args[0];

    char* creds = args[1]; // username:password
    strcat(packetMessage, creds);

    Device* device = g_hash_table_lookup(componentRegistry.devices, entityId);
    char* fromAddress = device->address;

    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entityId);
    packet_queue_write(&packet_buffer->sendQ, packet_alloc(entityId, fromAddress, toAddress, packetMessage));
}

