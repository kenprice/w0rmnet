#include "world_map.h"
#include "../components/component_registry.h"
#include "../entities/machine.h"
#include "../entities/router.h"

WorldMap worldMap;

void initialize_world() {
    ///////////////////////////////
    /// REGION
    ///////////////////////////////
    worldMap.numRegions = 1;
    strcpy(worldMap.regions[0].regionId, "nightcity");
    strcpy(worldMap.regions[0].regionLabel, "Night City");

    ///////////////////////////////
    /// ZONE
    worldMap.regions[0].numZones = 1;
    strcpy(worldMap.regions[0].zones[0].zoneId, "metro");
    strcpy(worldMap.regions[0].zones[0].zoneLabel, "Night City Metro Zone");
    strcpy(worldMap.regions[0].zones[0].parentRegion, "nightcity");
    worldMap.regions[0].zones[0].numAreas = 1;

    ///////////////////////////////
    /// PLAYER AREA
    ///////////////////////////////
    Area* playerArea = &worldMap.regions[0].zones[0].areas[0];
    worldMap.playerArea = playerArea;
    strcpy(playerArea->areaId, generate_uuid());
    strcpy(playerArea->areaName, "Your House");
    strcpy(playerArea->parentRegionId, "nightcity");
    strcpy(playerArea->parentZoneId, "metro");
    playerArea->height = 12;
    playerArea->width = 12;

    playerArea->numEntities = 5;
    Router* areaRouter = entity_router_create_blank();
    Router* router = entity_router_create_blank();
    Machine* machine1 = entity_machine_create_blank();
    Machine* machine2 = entity_machine_create_blank();
    Machine* machine3 = entity_machine_create_blank();
    strncpy(playerArea->entities[0], machine1->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[1], machine2->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[2], machine3->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[3], router->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[4], areaRouter->entityId, UUID_STR_LEN);

    ///////////////////////////////
    /// DEVICES
    ///////////////////////////////

    // Area Router
    // ============
    areaRouter->position.coord = (Vector2){4, 1};
    strcpy(areaRouter->device.address, "nightcity.metro.");
    strcat(areaRouter->device.address, areaRouter->device.name);
    areaRouter->device.owner = DEVICE_OWNER_PLAYER;
    areaRouter->device.visible = 1;
    areaRouter->connection.numConns = 2;
    strcpy(areaRouter->connection.toEntityIds[0], machine1->entityId);
    strcpy(areaRouter->connection.toEntityIds[1], router->entityId);
    entity_router_register_components(entity_router_deserialize(entity_router_serialize(*areaRouter)));

    // Router
    // ============
    router->position.coord = (Vector2){7, 4};
    strcpy(router->device.address, areaRouter->device.address);
    strcat(router->device.address, ".");
    strcat(router->device.address, router->device.name);
    router->device.owner = DEVICE_OWNER_PLAYER;
    router->device.visible = 1;
    router->connection.numConns = 3;
    strcpy(router->connection.parentEntityId, areaRouter->entityId);
    strcpy(router->connection.toEntityIds[0], areaRouter->entityId);
    strcpy(router->connection.toEntityIds[1], machine2->entityId);
    strcpy(router->connection.toEntityIds[2], machine3->entityId);
    entity_router_register_components(entity_router_deserialize(entity_router_serialize(*router)));

    // Machine 1
    // ============
    machine1->position.coord = (Vector2){2, 3};
    strcpy(machine1->device.address, areaRouter->device.address);
    strcat(machine1->device.address, ".");
    strcat(machine1->device.address, machine1->device.name);
    machine1->device.owner = DEVICE_OWNER_PLAYER;
    machine1->device.visible = 1;
    machine1->connection.numConns = 1;
    machine1->connection.maxConns = 1;
    strcpy(machine1->connection.parentEntityId, areaRouter->entityId);
    strcpy(machine1->connection.toEntityIds[0], areaRouter->entityId);
    // Processes
    machine1->processManager.maxProcs = 10;
    machine1->processManager.numProcs = 3;
    machine1->processManager.processes[0].type = PROCESS_TYPE_PING;
    machine1->processManager.processes[0].invocable = true;
    memset(machine1->processManager.processes[0].state, '\0', PROCESS_STATE_LEN);
    machine1->processManager.processes[1].type = PROCESS_TYPE_SCAN;
    machine1->processManager.processes[1].invocable = true;
    memset(machine1->processManager.processes[1].state, '\0', PROCESS_STATE_LEN);
    machine1->processManager.processes[2].type = PROCESS_TYPE_LOGIN;
    machine1->processManager.processes[2].invocable = true;
    memset(machine1->processManager.processes[2].state, '\0', PROCESS_STATE_LEN);
    // Register
    entity_machine_register_components(entity_machine_deserialize(entity_machine_serialize(*machine1)));

    // Machine 2
    // ============
    machine2->position.coord = (Vector2){7, 6};
    strcpy(machine2->device.address, router->device.address);
    strcat(machine2->device.address, ".");
    strcat(machine2->device.address, machine2->device.name);
    machine2->connection.numConns = 1;
    machine2->connection.maxConns = 1;
    strcpy(machine2->connection.parentEntityId, router->entityId);
    strcpy(machine2->connection.toEntityIds[0], router->entityId);
    // Processes
    machine2->processManager.maxProcs = 10;
    machine2->processManager.numProcs = 1;
    machine2->processManager.processes[0].type = PROCESS_TYPE_PING;
    machine2->processManager.processes[0].invocable = true;
    memset(machine2->processManager.processes[0].state, '\0', PROCESS_STATE_LEN);
    machine2->processManager.processes[0].state[0] = 1;
    // Register
    entity_machine_register_components(entity_machine_deserialize(entity_machine_serialize(*machine2)));

    // Machine 3
    // ============
    machine3->position.coord = (Vector2){9, 6};
    strcpy(machine3->device.address, router->device.address);
    strcat(machine3->device.address, ".");
    strcat(machine3->device.address, machine3->device.name);
    machine3->connection.numConns = 1;
    machine3->connection.maxConns = 1;
    strcpy(machine3->connection.parentEntityId, router->entityId);
    strcpy(machine3->connection.toEntityIds[0], router->entityId);
    // Processes
    machine3->processManager.maxProcs = 10;
    machine3->processManager.numProcs = 3;
    machine3->processManager.processes[0].type = PROCESS_TYPE_PING;
    machine3->processManager.processes[0].invocable = true;
    memset(machine3->processManager.processes[0].state, '\0', PROCESS_STATE_LEN);
    machine3->processManager.processes[1].type = PROCESS_TYPE_SCAN;
    machine3->processManager.processes[1].invocable = true;
    memset(machine3->processManager.processes[1].state, '\0', PROCESS_STATE_LEN);
    machine3->processManager.processes[2].type = PROCESS_TYPE_LOGIN;
    machine3->processManager.processes[2].invocable = true;
    memset(machine3->processManager.processes[2].state, '\0', PROCESS_STATE_LEN);
    strcpy(machine3->processManager.processes[2].state, "root:root");
    entity_machine_register_components(entity_machine_deserialize(entity_machine_serialize(*machine3)));

//    SaveFileText("world.sav", buffer);
}