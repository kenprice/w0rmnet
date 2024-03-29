#include <glib.h>
#include <string.h>
#include "wire.h"
#include "component_registry.h"
#include "device.h"

void register_wire(Wire wire, char* entityId) {
    Wire* newWire = calloc(1, sizeof(Wire));
    strncpy(newWire->entityA, wire.entityA, UUID_STR_LEN);
    strncpy(newWire->entityB, wire.entityB, UUID_STR_LEN);
    newWire->sendQtoA = packet_queue_alloc(200);
    newWire->sendQtoB = packet_queue_alloc(200);

    g_hash_table_insert(componentRegistry.wires, entityId, newWire);
}

char* create_and_register_wire(char* entityA, char* entityB) {
    Wire* newWire = calloc(1, sizeof(Wire));
    strncpy(newWire->entityA, entityA, UUID_STR_LEN);
    strncpy(newWire->entityB, entityB, UUID_STR_LEN);
    newWire->sendQtoA = packet_queue_alloc(200);
    newWire->sendQtoB = packet_queue_alloc(200);

    char* entityId = generate_uuid();
    g_hash_table_insert(componentRegistry.wires, entityId, newWire);

    return entityId;
}

Wire* search_wire_by_entity_ids(char* entityId1, char* entityId2) {
    GHashTableIter iter;
    char* entityId;
    Wire* wire;

    g_hash_table_iter_init(&iter, componentRegistry.wires);
    while (g_hash_table_iter_next (&iter, (gpointer) &entityId, (gpointer) &wire)) {
        if (strcmp(wire->entityA, entityId1) == 0 && strcmp(wire->entityB, entityId2)) {
            return wire;
        }
        if (strcmp(wire->entityB, entityId1) == 0 && strcmp(wire->entityA, entityId2)) {
            return wire;
        }
    }

    return NULL;
}

char* comp_wire_get_other_entity(char* wireEntityId, char* thisEntityId) {
    Wire* wire = g_hash_table_lookup(componentRegistry.wires, wireEntityId);
    if (!wire) return NULL;

    if (strcmp(wire->entityA, thisEntityId) == 0) {
        return wire->entityB;
    } else if (strcmp(wire->entityB, thisEntityId) == 0) {
        return wire->entityA;
    } else {
        return NULL;
    }
}
