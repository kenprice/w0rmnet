#include <glib.h>
#include "check_origin_runner.h"
#include "../../components/component_registry.h"

static void apply_firewall(PacketQueue* packetQueue, char** whitelist);

void proc_check_origin_handle_service(char* entityId, Process* process) {
    Wire* wire = g_hash_table_lookup(componentRegistry.wires, entityId);
    if (!wire) return;

    // State: address1,address2;address3,address4
    char** splitData = g_strsplit(process->state, ";", 2);

    char** whitelistA = g_strsplit(splitData[0], ",", 10);
    char** whitelistB = g_strsplit(splitData[1], ",", 10);

    apply_firewall(&wire->sendQtoA, whitelistA);

    apply_firewall(&wire->sendQtoB, whitelistB);

    g_strfreev(whitelistA);
    g_strfreev(whitelistB);
    g_strfreev(splitData);
}

static void apply_firewall(PacketQueue* packetQueue, char** whitelist) {
    Packet* packet = packet_queue_peek(packetQueue);
    if (!packet || !whitelist) return;

    bool found = false;
    for (int i = 0; whitelist[i] != NULL; i++) {
        char* whitelistedAddress = whitelist[i];
        if (strcmp(packet->fromAddress, whitelistedAddress) == 0) {
            found = true;
            break;
        }
    }
    if (!found) {
        // Consume packet without using it, to delete blocked packet
        packet_queue_read(packetQueue);
    }
}
