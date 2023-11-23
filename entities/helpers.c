#include <glib.h>
#include "helpers.h"
#include "../components/component_registry.h"
#include "../components/route_table.h"
#include "../components/wire.h"

/**
 * Send an outbound packet from some entity with a RouteTable and Wire
 *
 * @param fromEntity
 * @param packet
 * @return success
 */
bool entity_send_packet_from_entity(char* fromEntity, Packet* packet) {
    Wire* wire = NULL;

    // Look up in route table
    RouteTable* routeTable = g_hash_table_lookup(componentRegistry.routeTable, fromEntity);
    if (!routeTable) return false;

    int starIdx = -1;
    int foundIdx = -1;
    for (int i = 0; i < routeTable->numRecords; i++) {
        char* prefix = routeTable->records[i].prefix;
        if (strcmp(prefix, "*") == 0) {
            starIdx = i;
        }
        if (strncmp(prefix, packet->toAddress, strlen(prefix)) == 0) {
            foundIdx = i;
        }
    }

    if (starIdx == -1 && foundIdx == -1) return false;

    if (foundIdx > -1) {
        wire = g_hash_table_lookup(componentRegistry.wires, routeTable->records[foundIdx].wireEntityId);
    } else if (starIdx > -1) {
        wire = g_hash_table_lookup(componentRegistry.wires, routeTable->records[starIdx].wireEntityId);
    }
    if (!wire) return false;

    if (strcmp(wire->entityA, fromEntity) == 0) {
        packet_queue_write(&wire->sendQtoB, packet);
        return true;
    } else if (strcmp(wire->entityB, fromEntity) == 0) {
        packet_queue_write(&wire->sendQtoA, packet);
        return true;
    }

    return false;
}
