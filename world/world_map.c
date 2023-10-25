#include <stdio.h>
#include "world_map.h"
#include "../components/component_registry.h"
#include "../entities/machine.h"
#include "../entities/router.h"
#include "../components/wire.h"

WorldMap worldMap;

void initialize_world() {
    Router* zoneRouter = entity_router_create_blank();
    Router* areaRouter = entity_router_create_blank();
    Router* router = entity_router_create_blank();
    Machine* machine1 = entity_machine_create_blank();
    Machine* machine2 = entity_machine_create_blank();
    Machine* machine3 = entity_machine_create_blank();

    // Some other area
    Area* playerArea = &worldMap.regions[0].zones[0].areas[0];
    Area* firstArea = &worldMap.regions[0].zones[0].areas[1];
    Router* firstAreaRouter = entity_router_create_blank();
    Machine* firstAreaMachine = entity_machine_create_blank();

    strcpy(playerArea->zoneRouterEntityId, zoneRouter->entityId);
    playerArea->zoneRouterCoord = (Vector2){4, -5};
    strcpy(firstArea->zoneRouterEntityId, zoneRouter->entityId);
    firstArea->zoneRouterCoord = (Vector2){4, -5};


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
    worldMap.regions[0].zones[0].numAreas = 2;

    zoneRouter->position.coord = (Vector2){0, 0};
    sprintf(zoneRouter->device.address, "nightcity.metro");
    sprintf(zoneRouter->device.name, "metro");
    zoneRouter->device.owner = DEVICE_OWNER_PLAYER;
    zoneRouter->device.visible = 1;
    zoneRouter->connection.numConns = 2;
    strcpy(zoneRouter->connection.toEntityIds[0], areaRouter->entityId);
    strcpy(zoneRouter->connection.toEntityIds[1], firstAreaRouter->entityId);
    entity_router_register_components(entity_router_deserialize(entity_router_serialize(*zoneRouter)));

    ///////////////////////////////
    /// PLAYER AREA
    ///////////////////////////////
    worldMap.playerArea = playerArea;
    strcpy(playerArea->areaId, generate_uuid());
    strcpy(playerArea->areaName, "Your House");
    strcpy(playerArea->parentRegionId, "nightcity");
    strcpy(playerArea->parentZoneId, "metro");
    playerArea->height = 12;
    playerArea->width = 12;

    playerArea->numEntities = 5;
    strncpy(playerArea->entities[0], machine1->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[1], machine2->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[2], machine3->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[3], router->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[4], areaRouter->entityId, UUID_STR_LEN);

    ////
    // Wire experiment
    // TODO: Serialize Wire
    ////
    char* wireZoneRouterAreaRouter = create_and_register_wire(areaRouter->entityId, zoneRouter->entityId);
    char* wireAreaRouterRouter = create_and_register_wire(areaRouter->entityId, router->entityId);
    char* wireAreaRouterMachine1 = create_and_register_wire(areaRouter->entityId, machine1->entityId);
    char* wireRouterMachine2 = create_and_register_wire(router->entityId, machine2->entityId);
    char* wireRouterMachine3 = create_and_register_wire(router->entityId, machine3->entityId);

    ///////////////////////////////
    /// DEVICES
    ///////////////////////////////
    strcpy(areaRouter->device.address, "nightcity.metro.");
    strcat(areaRouter->device.address, areaRouter->device.name);
    strcpy(router->device.address, areaRouter->device.address);
    strcat(router->device.address, ".");
    strcat(router->device.address, router->device.name);
    strcpy(machine1->device.address, areaRouter->device.address);
    strcat(machine1->device.address, ".");
    strcat(machine1->device.address, machine1->device.name);
    strcpy(machine2->device.address, router->device.address);
    strcat(machine2->device.address, ".");
    strcat(machine2->device.address, machine2->device.name);
    strcpy(machine3->device.address, router->device.address);
    strcat(machine3->device.address, ".");
    strcat(machine3->device.address, machine3->device.name);

    // Area Router
    // ============
    areaRouter->position.coord = (Vector2){4, 1};
    areaRouter->device.owner = DEVICE_OWNER_PLAYER;
    areaRouter->device.visible = 1;
    areaRouter->connection.numConns = 3;
    strcpy(areaRouter->connection.parentEntityId, zoneRouter->entityId);
    strcpy(areaRouter->connection.toEntityIds[0], zoneRouter->entityId);
    strcpy(areaRouter->connection.toEntityIds[1], machine1->entityId);
    strcpy(areaRouter->connection.toEntityIds[2], router->entityId);
    areaRouter->routeTable.numRecords = 3;
    strcpy(areaRouter->routeTable.records[0].prefix, "*");
    strcpy(areaRouter->routeTable.records[0].wireEntityId, wireZoneRouterAreaRouter);
    strcpy(areaRouter->routeTable.records[1].prefix, router->device.address);
    strcpy(areaRouter->routeTable.records[1].wireEntityId, wireAreaRouterRouter);
    strcpy(areaRouter->routeTable.records[2].prefix, machine1->device.address);
    strcpy(areaRouter->routeTable.records[2].wireEntityId, wireAreaRouterMachine1);
    entity_router_register_components(entity_router_deserialize(entity_router_serialize(*areaRouter)));

    // Router
    // ============
    router->position.coord = (Vector2){7, 4};
    router->device.owner = DEVICE_OWNER_PLAYER;
    router->device.visible = 1;
    router->connection.numConns = 3;
    strcpy(router->connection.parentEntityId, areaRouter->entityId);
    strcpy(router->connection.toEntityIds[0], areaRouter->entityId);
    strcpy(router->connection.toEntityIds[1], machine2->entityId);
    strcpy(router->connection.toEntityIds[2], machine3->entityId);
    router->routeTable.numRecords = 3;
    strcpy(router->routeTable.records[0].prefix, "*");
    strcpy(router->routeTable.records[0].wireEntityId, wireAreaRouterRouter);
    strcpy(router->routeTable.records[1].prefix, machine2->device.address);
    strcpy(router->routeTable.records[1].wireEntityId, wireRouterMachine2);
    strcpy(router->routeTable.records[2].prefix, machine3->device.address);
    strcpy(router->routeTable.records[2].wireEntityId, wireRouterMachine3);
    entity_router_register_components(entity_router_deserialize(entity_router_serialize(*router)));

    // Machine 1
    // ============
    machine1->position.coord = (Vector2){2, 3};
    machine1->device.owner = DEVICE_OWNER_PLAYER;
    machine1->device.visible = 1;
    machine1->connection.numConns = 1;
    machine1->connection.maxConns = 1;
    strcpy(machine1->connection.parentEntityId, areaRouter->entityId);
    strcpy(machine1->connection.toEntityIds[0], areaRouter->entityId);
    machine1->routeTable.numRecords = 1;
    strcpy(machine1->routeTable.records[0].prefix, "*");
    strcpy(machine1->routeTable.records[0].wireEntityId, wireAreaRouterMachine1);
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
    machine2->connection.numConns = 1;
    machine2->connection.maxConns = 1;
    strcpy(machine2->connection.parentEntityId, router->entityId);
    strcpy(machine2->connection.toEntityIds[0], router->entityId);
    machine2->routeTable.numRecords = 1;
    strcpy(machine2->routeTable.records[0].prefix, "*");
    strcpy(machine2->routeTable.records[0].wireEntityId, wireRouterMachine2);
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
    machine3->connection.numConns = 1;
    machine3->connection.maxConns = 1;
    strcpy(machine3->connection.parentEntityId, router->entityId);
    strcpy(machine3->connection.toEntityIds[0], router->entityId);
    machine3->routeTable.numRecords = 1;
    strcpy(machine3->routeTable.records[0].prefix, "*");
    strcpy(machine3->routeTable.records[0].wireEntityId, wireRouterMachine3);
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


    ///////////////////////////////
    /// SOME OTHER AREA
    ///////////////////////////////
    strcpy(firstArea->areaId, generate_uuid());
    strcpy(firstArea->areaName, "Somearea");
    strcpy(firstArea->parentRegionId, "nightcity");
    strcpy(firstArea->parentZoneId, "metro");
    firstArea->height = 12;
    firstArea->width = 12;

    firstArea->numEntities = 2;
    strncpy(firstArea->entities[0], firstAreaRouter->entityId, UUID_STR_LEN);
    strncpy(firstArea->entities[1], firstAreaMachine->entityId, UUID_STR_LEN);

    // Area Router
    // ============
    firstAreaRouter->position.coord = (Vector2){4, 1};
    strcpy(firstAreaRouter->device.address, "nightcity.metro.");
    strcat(firstAreaRouter->device.address, firstAreaRouter->device.name);
    firstAreaRouter->device.owner = DEVICE_OWNER_NOBODY;
    firstAreaRouter->device.visible = 1;
    firstAreaRouter->connection.numConns = 2;
    strcpy(firstAreaRouter->connection.parentEntityId, zoneRouter->entityId);
    strcpy(firstAreaRouter->connection.toEntityIds[0], zoneRouter->entityId);
    strcpy(firstAreaRouter->connection.toEntityIds[1], firstAreaMachine->entityId);
    entity_router_register_components(entity_router_deserialize(entity_router_serialize(*firstAreaRouter)));

    // Machine
    // ============
    firstAreaMachine->position.coord = (Vector2){2, 3};
    strcpy(firstAreaMachine->device.address, firstAreaRouter->device.address);
    strcat(firstAreaMachine->device.address, ".");
    strcat(firstAreaMachine->device.address, firstAreaMachine->device.name);
    firstAreaMachine->device.owner = DEVICE_OWNER_NOBODY;
    firstAreaMachine->device.visible = 1;
    firstAreaMachine->connection.numConns = 1;
    firstAreaMachine->connection.maxConns = 1;
    strcpy(firstAreaMachine->connection.parentEntityId, firstAreaRouter->entityId);
    strcpy(firstAreaMachine->connection.toEntityIds[0], firstAreaRouter->entityId);
    // Processes
    firstAreaMachine->processManager.maxProcs = 10;
    firstAreaMachine->processManager.numProcs = 1;
    firstAreaMachine->processManager.processes[0].type = PROCESS_TYPE_PING;
    firstAreaMachine->processManager.processes[0].invocable = true;
    memset(firstAreaMachine->processManager.processes[0].state, '\0', PROCESS_STATE_LEN);
    entity_machine_register_components(entity_machine_deserialize(entity_machine_serialize(*firstAreaMachine)));

//    SaveFileText("world.sav", buffer);
}

bool is_entity_in_area(Area area, char* entityId) {
    for (int i = 0; i < area.numEntities; i++) {
        if (strcmp(area.entities[i], entityId)) {
            return true;
        }
    }
    return false;
}
