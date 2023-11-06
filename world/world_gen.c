#include <glib.h>
#include <stdio.h>
#include <string.h>
#include "../components/component_registry.h"
#include "world_map.h"
#include "world_gen.h"
#include "../utils/uuid.h"
#include "../utils/random.h"

static void connect_machine_to_router(Area* area, Polygon wirePolygon, char* machineId, char* routerId);

static void populate_processes(char* machineId);

/**
 * @param addressPrefix
 * @param area
 * @param x Starting point x
 * @param y Starting point y
 * @returns entityId of router
 */
char* generate_subnet_7x7(char* addressPrefix, Area* area, int x, int y) {
    char addressBuffer[110];

    // Offsets for device placement
    const int offsetX[7] = { -3, -2, -1, 0, 1, 2, 3 };
    const int offsetY[7] = { 4, 2, 4, 2, 4, 2, 4 };

    // Router
    Router* router = entity_router_create_blank();
    strcpy(router->device.entityId, router->entityId);
    sprintf(addressBuffer, "%s.%s", addressPrefix, router->device.name);
    strcpy(router->device.address, addressBuffer);
    // Position
    router->position.coord.x = x;
    router->position.coord.y = y;
    router->device.visible = true;
    entity_router_register_components(router);
    strcpy(area->entities[area->numEntities++], router->entityId);

    for (int i = 0; i < 7; i++) {
        // Machine
        Machine* machine = entity_machine_create_blank();
        strcpy(machine->device.entityId, machine->entityId);
        sprintf(addressBuffer, "%s.%s", router->device.address, machine->device.name);
        strcpy(machine->device.address, addressBuffer);
        machine->position.coord.x = x + offsetX[i];
        machine->position.coord.y = y + offsetY[i];
//        machine->device.visible = true;
        entity_machine_register_components(machine);
        strcpy(area->entities[area->numEntities++], machine->entityId);
        populate_processes(machine->entityId);

        // Wire
        Polygon wirePolygon = comp_polygon_points4(x, y, x, y+1, x+offsetX[i], y+1, x+offsetX[i], y+offsetY[i]);
        connect_machine_to_router(area, wirePolygon, machine->device.entityId, router->device.entityId);
    }

    return router->entityId;
}

void generate_area_19x19(Region* region, Zone* zone, Area* area, const char* areaId, const char* areaName) {
    char addressPrefix[110];
    sprintf(addressPrefix, "%s.%s", region->regionId, zone->zoneId);

    strcpy(area->areaId, areaId);
    strcpy(area->areaName, areaName);
    area->height = 19;
    area->width = 19;
    area->numEntities = 0;

    // Area router
    Polygon wirePolygon;

    Router* router = entity_router_create_blank();
    strcpy(router->device.entityId, router->entityId);
    sprintf(router->device.address, "%s.%s", addressPrefix, router->device.name);
    router->device.visible = true;
    router->position.coord.x = 9;
    router->position.coord.y = 1;
    entity_router_register_components(router);
    strcpy(area->entities[area->numEntities++], router->entityId);
    wirePolygon = comp_polygon_points2(9, 1, 9, -2);
    connect_machine_to_router(area, wirePolygon, router->entityId, zone->gateway);

    // Subnets
    char* subnetRouterId;
    subnetRouterId = generate_subnet_7x7(router->device.address, area, 4, 3);
    wirePolygon = comp_polygon_points4(9, 1, 9, 2, 4, 2, 4, 3);
    connect_machine_to_router(area, wirePolygon, subnetRouterId, router->entityId);

    subnetRouterId = generate_subnet_7x7(router->device.address, area, 14, 3);
    wirePolygon = comp_polygon_points4(9, 1, 9, 2, 14, 2, 14, 3);
    connect_machine_to_router(area, wirePolygon, subnetRouterId, router->entityId);

    subnetRouterId = generate_subnet_7x7(router->device.address, area, 4, 11);
    wirePolygon = comp_polygon_points4(9, 1, 9, 10, 4, 10, 4, 11);
    connect_machine_to_router(area, wirePolygon, subnetRouterId, router->entityId);

    subnetRouterId = generate_subnet_7x7(router->device.address, area, 14, 11);
    wirePolygon = comp_polygon_points4(9, 1, 9, 10, 14, 10, 14, 11);
    connect_machine_to_router(area, wirePolygon, subnetRouterId, router->entityId);
}

static void connect_machine_to_router(Area* area, Polygon wirePolygon, char* machineId, char* routerId) {
    Device* machine = g_hash_table_lookup(componentRegistry.devices, machineId);

    // Create Wire
    char* wireId = create_and_register_wire(machineId, routerId);
    register_polygon(wirePolygon, wireId);
    strcpy(area->entities[area->numEntities++], wireId);

    // Route Table
    RouteTable* machineRouteTable = g_hash_table_lookup(componentRegistry.routeTable, machineId);
    int machineRecIdx = machineRouteTable->numRecords;
    strcpy(machineRouteTable->records[machineRecIdx].prefix, "*");
    strcpy(machineRouteTable->records[machineRecIdx].wireEntityId, wireId);
    machineRouteTable->numRecords++;

    RouteTable* routerRouteTable = g_hash_table_lookup(componentRegistry.routeTable, routerId);
    int routerRecIdx = routerRouteTable->numRecords;
    strcpy(routerRouteTable->records[routerRecIdx].prefix, machine->address);
    strcpy(routerRouteTable->records[routerRecIdx].wireEntityId, wireId);
    routerRouteTable->numRecords++;
}

static void populate_processes(char* machineId) {
    ProcessManager* processManager = g_hash_table_lookup(componentRegistry.processManagers, machineId);
    processManager->maxProcs = 10;

    int i = 0;

    processManager->processes[i].type = PROCESS_TYPE_PING;
    if (!randint(2)) {
        processManager->processes[i].invocable = true;
    }
    if (!randint(2)) {
        processManager->processes[i].state[0] = 1;
    }

    if (randint(5)) {
        i++;
        processManager->processes[i].type = PROCESS_TYPE_LOGIN;
        processManager->processes[i].invocable = true;
        strcpy(processManager->processes[i].state, "root:root");
    }

    processManager->numProcs = ++i;
}
