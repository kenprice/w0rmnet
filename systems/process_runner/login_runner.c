#include "glib.h"
#include "login_runner.h"
#include "../../components/component_registry.h"
#include "../utils/routing.h"

static char* resolve_prog_target_device_address(char* entityId, char* targetAddress) {
    Device* fromDevice = (Device*)g_hash_table_lookup(componentRegistry.devices, entityId);
    Device* targetDevice = search_device_by_relative_address(entityId, targetAddress);
    if (!targetDevice) return NULL;

    char** splitOriginAddress = g_strsplit(fromDevice->address, ".", 10);
    char** splitTargetAddress = g_strsplit(targetDevice->address, ".", 10);
    char simplifiedTargetAddress[100] = "";

    int i;
    for (i = 0; i < 10; i++) {
        if (splitOriginAddress[i] == NULL || splitTargetAddress[i] == NULL) {
            break;
        }
        if (strcmp(splitOriginAddress[i], splitTargetAddress[i]) != 0) {
            break;
        }
    }
    for (int j = i-1; j < 10; j++) {
        if (splitTargetAddress[j] != NULL && strlen(splitTargetAddress[j])) {
            strcat(simplifiedTargetAddress, splitTargetAddress[j]);
            strcat(simplifiedTargetAddress, ".");
        } else {
            break;
        }
    }
    simplifiedTargetAddress[strlen(simplifiedTargetAddress)-1] = '\0';

    char* returnTargetAddress = calloc(strlen(simplifiedTargetAddress)+1, sizeof(char));
    strcpy(returnTargetAddress, simplifiedTargetAddress);
    return returnTargetAddress;
}

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

