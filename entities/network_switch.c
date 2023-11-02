#include <string.h>
#include <glib.h>
#include "network_switch.h"
#include "../graphics/sprites.h"
#include "../components/component_registry.h"

NetworkSwitch* entity_network_switch_create_blank() {
    NetworkSwitch* networkSwitch = calloc(1, sizeof(NetworkSwitch));
    char* entity_id = generate_uuid();
    strcpy(networkSwitch->entityId, entity_id);

    char* device_id = generate_device_id();
    networkSwitch->device.type = DEVICE_TYPE_GENERIC;
    networkSwitch->device.owner = DEVICE_OWNER_NOBODY;
    networkSwitch->device.pwned = DEVICE_NOT_PWNED;
    strncpy(networkSwitch->device.entityId, entity_id, UUID_STR_LEN);
    strncpy(networkSwitch->device.name, device_id, DEVICE_NAME_LEN);

    networkSwitch->position.coord.x = 0;
    networkSwitch->position.coord.y = 0;

    networkSwitch->sprite.spriteId = SPRITE_SWITCH_GENERIC;

    networkSwitch->wire.sendQtoA = packet_queue_alloc(50);
    networkSwitch->wire.sendQtoB = packet_queue_alloc(50);

    return networkSwitch;
}

char* entity_network_switch_register_components(NetworkSwitch networkSwitch) {
    char* entityId = strdup(networkSwitch.entityId);

    register_device(networkSwitch.device, entityId);

    Position* position = calloc(1, sizeof(Position));
    memcpy(position, &networkSwitch.position, sizeof(Position));
    g_hash_table_insert(componentRegistry.positions, entityId, position);

    Sprite* sprite = calloc(1, sizeof(Sprite));
    memcpy(sprite, &networkSwitch.sprite, sizeof(Sprite));
    g_hash_table_insert(componentRegistry.sprites, entityId, sprite);

    Polygon* polygon = calloc(1, sizeof(Polygon));
    memcpy(polygon, &networkSwitch.polygon, sizeof(Polygon));
    g_hash_table_insert(componentRegistry.polygons, entityId, polygon);

    Wire* wire = calloc(1, sizeof(Wire));
    memcpy(wire, &networkSwitch.wire, sizeof(Wire));
    g_hash_table_insert(componentRegistry.wires, entityId, wire);

    ProcessManager* processManager = calloc(1, sizeof(ProcessManager));
    processManager->numProcs = networkSwitch.processManager.numProcs;
    processManager->maxProcs = networkSwitch.processManager.maxProcs;
    for (int i = 0; i < networkSwitch.processManager.numProcs; i++) {
        processManager->processes[i].type = networkSwitch.processManager.processes[i].type;
        processManager->processes[i].invocable = networkSwitch.processManager.processes[i].invocable;
        strcpy(processManager->processes[i].state, networkSwitch.processManager.processes[i].state);
    }
    g_hash_table_insert(componentRegistry.processManagers, entityId, processManager);
    register_proc_msg_queue(proc_msg_queue_alloc(20), entityId);

    return entityId;
}
