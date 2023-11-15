#include <stdio.h>
#include "world_map.h"
#include "world_gen.h"
#include "../components/component_registry.h"
#include "../entities/machine.h"
#include "../entities/router.h"
#include "../entities/network_switch.h"
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
    strcpy(worldMap.regions[0].zones[0].gateway, zoneRouter->entityId);

    zoneRouter->position.coord = (Vector2){0, 0};
    sprintf(zoneRouter->device.address, "nightcity.metro");
    sprintf(zoneRouter->device.name, "metro");
    zoneRouter->device.visible = 1;
    zoneRouter->routeTable.numRecords = 1;
    strcpy(zoneRouter->routeTable.records[0].prefix, areaRouter->device.address);
    strcpy(zoneRouter->routeTable.records[0].wireEntityId, wireZoneRouterAreaRouter);
    entity_router_register_components(zoneRouter);



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

    playerArea->numEntities = 10;
    strncpy(playerArea->entities[0], machine1->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[1], machine2->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[2], machine3->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[3], router->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[4], areaRouter->entityId, UUID_STR_LEN);
    strncpy(playerArea->entities[5], wireAreaRouterRouter, UUID_STR_LEN);
    strncpy(playerArea->entities[6], wireAreaRouterMachine1, UUID_STR_LEN);
    strncpy(playerArea->entities[7], wireRouterMachine2, UUID_STR_LEN);
    strncpy(playerArea->entities[8], wireRouterMachine3, UUID_STR_LEN);
    strncpy(playerArea->entities[9], wireZoneRouterAreaRouter, UUID_STR_LEN);

    ///////////////////////////////
    /// DEVICES
    ///////////////////////////////

    // Area Router
    // ============
    areaRouter->position.coord = (Vector2){4, 1};
    areaRouter->device.owner = DEVICE_OWNER_PLAYER;
    areaRouter->device.visible = 1;
    areaRouter->routeTable.numRecords = 3;
    strcpy(areaRouter->routeTable.records[0].prefix, "*");
    strcpy(areaRouter->routeTable.records[0].wireEntityId, wireZoneRouterAreaRouter);
    strcpy(areaRouter->routeTable.records[1].prefix, router->device.address);
    strcpy(areaRouter->routeTable.records[1].wireEntityId, wireAreaRouterRouter);
    strcpy(areaRouter->routeTable.records[2].prefix, machine1->device.address);
    strcpy(areaRouter->routeTable.records[2].wireEntityId, wireAreaRouterMachine1);
    entity_router_register_components(areaRouter);

    // Router
    // ============
    router->position.coord = (Vector2){7, 4};
    router->device.owner = DEVICE_OWNER_PLAYER;
    router->device.visible = 1;
    router->routeTable.numRecords = 3;
    strcpy(router->routeTable.records[0].prefix, "*");
    strcpy(router->routeTable.records[0].wireEntityId, wireAreaRouterRouter);
    strcpy(router->routeTable.records[1].prefix, machine2->device.address);
    strcpy(router->routeTable.records[1].wireEntityId, wireRouterMachine2);
    strcpy(router->routeTable.records[2].prefix, machine3->device.address);
    strcpy(router->routeTable.records[2].wireEntityId, wireRouterMachine3);
    entity_router_register_components(router);

    // Machine 1
    // ============
    machine1->position.coord = (Vector2){2, 3};
    machine1->device.owner = DEVICE_OWNER_PLAYER;
    machine1->device.visible = 1;
    machine1->routeTable.numRecords = 1;
    strcpy(machine1->routeTable.records[0].prefix, "*");
    strcpy(machine1->routeTable.records[0].wireEntityId, wireAreaRouterMachine1);
    // Processes
    machine1->processManager.maxProcs = 10;
    machine1->processManager.numProcs = 3;
    machine1->processManager.processes[0].program = PingPrograms[0];
    machine1->processManager.processes[0].invocable = true;
    memset(machine1->processManager.processes[0].state, '\0', PROCESS_STATE_LEN);
    machine1->processManager.processes[1].program = ScanPrograms[0];
    machine1->processManager.processes[1].invocable = true;
    memset(machine1->processManager.processes[1].state, '\0', PROCESS_STATE_LEN);
    machine1->processManager.processes[2].program = LoginPrograms[0];
    machine1->processManager.processes[2].invocable = true;
    memset(machine1->processManager.processes[2].state, '\0', PROCESS_STATE_LEN);
    // Register
    entity_machine_register_components(machine1);

    // Machine 2
    // ============
    machine2->position.coord = (Vector2){7, 6};
    machine2->routeTable.numRecords = 1;
    strcpy(machine2->routeTable.records[0].prefix, "*");
    strcpy(machine2->routeTable.records[0].wireEntityId, wireRouterMachine2);
    // Processes
    machine2->processManager.maxProcs = 10;
    machine2->processManager.numProcs = 1;
    machine2->processManager.processes[0].program = PingPrograms[0];
    machine2->processManager.processes[0].invocable = true;
    memset(machine2->processManager.processes[0].state, '\0', PROCESS_STATE_LEN);
    machine2->processManager.processes[0].state[0] = 1;
    // Register
    entity_machine_register_components(machine2);

    // Machine 3
    // ============
    machine3->position.coord = (Vector2){9, 6};
    machine3->routeTable.numRecords = 1;
    strcpy(machine3->routeTable.records[0].prefix, "*");
    strcpy(machine3->routeTable.records[0].wireEntityId, wireRouterMachine3);
    // Processes
    machine3->processManager.maxProcs = 10;
    machine3->processManager.numProcs = 3;
    machine3->processManager.processes[0].program = PingPrograms[0];
    machine3->processManager.processes[0].invocable = true;
    memset(machine3->processManager.processes[0].state, '\0', PROCESS_STATE_LEN);
    machine3->processManager.processes[1].program = ScanPrograms[0];
    machine3->processManager.processes[1].invocable = true;
    memset(machine3->processManager.processes[1].state, '\0', PROCESS_STATE_LEN);
    machine3->processManager.processes[2].program = LoginPrograms[0];
    machine3->processManager.processes[2].invocable = true;
    memset(machine3->processManager.processes[2].state, '\0', PROCESS_STATE_LEN);
    strcpy(machine3->processManager.processes[2].state, "root:root");
    entity_machine_register_components(machine3);

    ///// Wire gfx
    Polygon polyZoneRouterAreaRouter;
    polyZoneRouterAreaRouter.points[0] = (PolyPoint){ areaRouter->position.coord.x, areaRouter->position.coord.y };
    polyZoneRouterAreaRouter.points[1] = (PolyPoint){ areaRouter->position.coord.x, areaRouter->position.coord.y - 4 };
    polyZoneRouterAreaRouter.numPoints = 2;
    register_polygon(polyZoneRouterAreaRouter, wireZoneRouterAreaRouter);
    Polygon polyAreaRouterRouter;
    polyAreaRouterRouter.points[0] = (PolyPoint){ areaRouter->position.coord.x, areaRouter->position.coord.y };
    polyAreaRouterRouter.points[1] = (PolyPoint){ router->position.coord.x, areaRouter->position.coord.y };
    polyAreaRouterRouter.points[2] = (PolyPoint){ router->position.coord.x, router->position.coord.y };
    polyAreaRouterRouter.numPoints = 3;
    register_polygon(polyAreaRouterRouter, wireAreaRouterRouter);
    Polygon polyAreaRouterMachine1;
    polyAreaRouterMachine1.points[0] = (PolyPoint){ areaRouter->position.coord.x, areaRouter->position.coord.y };
    polyAreaRouterMachine1.points[1] = (PolyPoint){ machine1->position.coord.x, areaRouter->position.coord.y };
    polyAreaRouterMachine1.points[2] = (PolyPoint){ machine1->position.coord.x, machine1->position.coord.y };
    polyAreaRouterMachine1.numPoints = 3;
    register_polygon(polyAreaRouterMachine1, wireAreaRouterMachine1);
    Polygon polyAreaRouterMachine2;
    polyAreaRouterMachine2.points[0] = (PolyPoint){ router->position.coord.x, router->position.coord.y };
    polyAreaRouterMachine2.points[1] = (PolyPoint){ machine2->position.coord.x, machine2->position.coord.y };
    polyAreaRouterMachine2.numPoints = 2;
    register_polygon(polyAreaRouterMachine2, wireRouterMachine2);
    Polygon polyAreaRouterMachine3;
    polyAreaRouterMachine3.points[0] = (PolyPoint){ router->position.coord.x, router->position.coord.y };
    polyAreaRouterMachine3.points[1] = (PolyPoint){ machine3->position.coord.x, router->position.coord.y };
    polyAreaRouterMachine3.points[2] = (PolyPoint){ machine3->position.coord.x, machine3->position.coord.y };
    polyAreaRouterMachine3.numPoints = 3;
    register_polygon(polyAreaRouterMachine3, wireRouterMachine3);


    worldMap.regions[0].zones[0].numAreas = 6;
    generate_area_19x19(&worldMap.regions[0], &worldMap.regions[0].zones[0], &worldMap.regions[0].zones[0].areas[1],
                        "test1", "Worldgen Test 1");
    generate_area_19x19(&worldMap.regions[0], &worldMap.regions[0].zones[0], &worldMap.regions[0].zones[0].areas[2],
                        "test2", "Worldgen Test 2");
    generate_area_19x19(&worldMap.regions[0], &worldMap.regions[0].zones[0], &worldMap.regions[0].zones[0].areas[3],
                        "test2", "Worldgen Test 3");
    generate_area_19x19(&worldMap.regions[0], &worldMap.regions[0].zones[0], &worldMap.regions[0].zones[0].areas[4],
                        "test2", "Worldgen Test 3");
    generate_area_19x19(&worldMap.regions[0], &worldMap.regions[0].zones[0], &worldMap.regions[0].zones[0].areas[5],
                        "test2", "Worldgen Test 3");


    // =================================
    // Network Switch Test
    NetworkSwitch* networkSwitch = entity_network_switch_create_blank();
    Machine* machine4 = entity_machine_create_blank();
    sprintf(networkSwitch->device.address, "%s.%s", areaRouter->device.address, networkSwitch->device.name);
    sprintf(machine4->device.address, "%s.%s", areaRouter->device.address, machine4->device.name);

    networkSwitch->device.type = DEVICE_TYPE_SWITCH;
    networkSwitch->device.visible = true;
    networkSwitch->device.owner = DEVICE_OWNER_PLAYER;
    networkSwitch->polygon.numPoints = 3;
    networkSwitch->polygon.points[0] = (PolyPoint){ 4, 1 };
    networkSwitch->polygon.points[1] = (PolyPoint){ 4, 3 };
    networkSwitch->polygon.points[2] = (PolyPoint){ 4, 5 };
    networkSwitch->polygon.bisectAt = 1;
    networkSwitch->position.coord = (Vector2){ 4, 3 };
    strcpy(networkSwitch->wire.entityA, areaRouter->entityId);
    strcpy(networkSwitch->wire.entityB, machine4->entityId);
    networkSwitch->processManager.numProcs = 1;
    networkSwitch->processManager.processes[0].program = CheckOriginPrograms[0];
    networkSwitch->processManager.processes[0].isService = true;
    memset(networkSwitch->processManager.processes[0].state, '\0', PROCESS_STATE_LEN);
    sprintf(networkSwitch->processManager.processes[0].state, "%s;%s", machine4->device.address, machine3->device.address);
    char* networkSwitchId = entity_network_switch_register_components(*networkSwitch);

    strcpy(worldMap.regions[0].zones[0].areas[0].entities[worldMap.regions[0].zones[0].areas[0].numEntities++], networkSwitchId);

    machine4->position.coord = (Vector2){4, 5};
    machine4->device.owner = DEVICE_OWNER_NOBODY;
    machine4->device.visible = 1;
    machine4->routeTable.numRecords = 1;
    strcpy(machine4->routeTable.records[0].prefix, "*");
    strcpy(machine4->routeTable.records[0].wireEntityId, networkSwitchId);
    // Processes
    machine4->processManager.maxProcs = 10;
    machine4->processManager.numProcs = 2;
    machine4->processManager.processes[0].program = PingPrograms[0];
    machine4->processManager.processes[0].invocable = true;
    memset(machine4->processManager.processes[0].state, '\0', PROCESS_STATE_LEN);
    machine4->processManager.processes[0].program = LoginPrograms[0];
    machine4->processManager.processes[1].invocable = true;
    memset(machine4->processManager.processes[1].state, '\0', PROCESS_STATE_LEN);
    strcpy(machine4->processManager.processes[1].state, "root:root");

    strcpy(areaRouter->routeTable.records[areaRouter->routeTable.numRecords].prefix, machine4->device.address);
    strcpy(areaRouter->routeTable.records[areaRouter->routeTable.numRecords++].wireEntityId, networkSwitch->entityId);

    entity_machine_register_components(machine4);
    strcpy(worldMap.regions[0].zones[0].areas[0].entities[worldMap.regions[0].zones[0].areas[0].numEntities++], machine4->entityId);



//    wireZoneRouterFirstArea = create_and_register_wire(firstAreaRouter->entityId, zoneRouter->entityId);
//    wireFirstAreaRouterMachine = create_and_register_wire(firstAreaRouter->entityId, firstAreaMachine->entityId);

//    SaveFileText("world.sav", buffer);
}

bool is_entity_in_area(Area* area, char* entityId) {
    for (int i = 0; i < area->numEntities; i++) {
        if (strcmp(area->entities[i], entityId) == 0) {
            return true;
        }
    }
    return false;
}

Area* find_area_by_device(Device* device) {
    for (int i = 0; i < worldMap.numRegions; i++) {
        Region* region = &worldMap.regions[i];
        for (int j = 0; j < region->numZones; j++) {
            Zone* zone = &region->zones[j];
            for (int k = 0; k < zone->numAreas; k++) {
                Area* area = &zone->areas[k];
                if (is_entity_in_area(area, device->entityId)) {
                    return area;
                }
            }
        }
    }
    return NULL;
}
