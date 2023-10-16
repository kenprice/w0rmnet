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

    // Top-level
    Router* areaRouter = entity_router_create_blank();
    areaRouter->position.coord = (Vector2){4, 1};
    strcpy(areaRouter->device.address, "nightcity.metro.");
    strcat(areaRouter->device.address, areaRouter->device.id);
    areaRouter->device.owner = DEVICE_OWNER_PLAYER;
    areaRouter->device.visible = 1;
    entity_router_register_components(*areaRouter);

    // LANs
    Router* router = entity_router_create_blank();
    router->position.coord = (Vector2){7, 4};
    strcpy(router->device.address, areaRouter->device.address);
    strcat(router->device.address, ".");
    strcat(router->device.address, router->device.id);
    router->device.owner = DEVICE_OWNER_PLAYER;
    router->device.visible = 1;
    entity_router_register_components(*router);
    connection_set_parent(router->entity_id, areaRouter->device.id);
    connection_add_device(router->entity_id, areaRouter->device.id);
    connection_add_device(areaRouter->entity_id, router->device.id);

    // Machines within LANs
    Machine* machine1 = entity_machine_create_blank();
    machine1->position.coord = (Vector2){2, 3};
    strcpy(machine1->device.address, areaRouter->device.address);
    strcat(machine1->device.address, ".");
    strcat(machine1->device.address, machine1->device.id);
    machine1->device.owner = DEVICE_OWNER_PLAYER;
    machine1->device.visible = 1;
    entity_machine_register_components(*machine1);
    connection_set_parent(machine1->entity_id, areaRouter->device.id);
    connection_add_device(machine1->entity_id, areaRouter->device.id);
    connection_add_device(areaRouter->entity_id, machine1->device.id);

    Machine* machine2 = entity_machine_create_blank();
    machine2->position.coord = (Vector2){7, 6};
    strcpy(machine2->device.address, router->device.address);
    strcat(machine2->device.address, ".");
    strcat(machine2->device.address, machine2->device.id);
    entity_machine_register_components(*machine2);
    connection_set_parent(machine2->entity_id, router->device.id);
    connection_add_device(machine2->entity_id, router->device.id);
    connection_add_device(router->entity_id, machine2->device.id);

    Machine* machine3 = entity_machine_create_blank();
    machine3->position.coord = (Vector2){9, 6};
    strcpy(machine3->device.address, router->device.address);
    strcat(machine3->device.address, ".");
    strcat(machine3->device.address, machine3->device.id);
    entity_machine_register_components(*machine3);
    connection_set_parent(machine3->entity_id, router->device.id);
    connection_add_device(machine3->entity_id, router->device.id);
    connection_add_device(router->entity_id, machine3->device.id);

    char address_buff[100] = "";
    address_buff[0] = '\0';
    strcat(address_buff, areaRouter->device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router->device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine3->device.id);

//    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine2->entity_id);
//    packet_queue_write(&packet_buffer->send_q, packet_alloc(address_buff, "Ping?"));

    ProcessManager process_manager;
    process_manager.max_procs = 10;
    process_manager.num_procs = 3;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invocable = true;
    memset(process_manager.processes[0].state, '\0', PROCESS_STATE_LEN);
    process_manager.processes[1].type = PROCESS_TYPE_SCAN;
    process_manager.processes[1].invocable = true;
    memset(process_manager.processes[1].state, '\0', PROCESS_STATE_LEN);
    process_manager.processes[2].type = PROCESS_TYPE_LOGIN;
    process_manager.processes[2].invocable = true;
    memset(process_manager.processes[2].state, '\0', PROCESS_STATE_LEN);
    register_process_manager(process_manager, machine1->entity_id);
    ProcMessageQueue machine1_pmq = proc_msg_queue_alloc(10);
    register_proc_msg_queue(machine1_pmq, machine1->entity_id);
    address_buff[0] = '\0';
    strcat(address_buff, areaRouter->device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router->device.id);
//    ProcMessage* msg = proc_msg_alloc(1, address_buff);
//    proc_msg_queue_write(g_hash_table_lookup(component_registry.proc_msg_queues, machine1->entity_id), msg);

    process_manager.max_procs = 10;
    process_manager.num_procs = 1;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invocable = true;
    memset(process_manager.processes[0].state, '\0', PROCESS_STATE_LEN);
    process_manager.processes[0].state[0] = 1;
    register_process_manager(process_manager, machine2->entity_id);
    ProcMessageQueue machine2_pmq = proc_msg_queue_alloc(10);
    register_proc_msg_queue(machine2_pmq, machine2->entity_id);
//    msg = proc_msg_alloc(0, address_buff);
//    proc_msg_queue_write(g_hash_table_lookup(component_registry.proc_msg_queues, machine2->entity_id), msg);

    process_manager.max_procs = 10;
    process_manager.num_procs = 3;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invocable = true;
    memset(process_manager.processes[0].state, '\0', PROCESS_STATE_LEN);
    process_manager.processes[1].type = PROCESS_TYPE_SCAN;
    process_manager.processes[1].invocable = true;
    memset(process_manager.processes[1].state, '\0', PROCESS_STATE_LEN);
    process_manager.processes[2].type = PROCESS_TYPE_LOGIN;
    process_manager.processes[2].invocable = true;
    memset(process_manager.processes[2].state, '\0', PROCESS_STATE_LEN);
    strcpy(process_manager.processes[2].state, "root:root");
    register_process_manager(process_manager, machine3->entity_id);
    register_proc_msg_queue(proc_msg_queue_alloc(10), machine3->entity_id);


    // Area info
    playerArea->numEntities = 5;
    strncpy(playerArea->entities[0], machine1->entity_id, UUID_STR_LEN);
    strncpy(playerArea->entities[1], machine2->entity_id, UUID_STR_LEN);
    strncpy(playerArea->entities[2], machine3->entity_id, UUID_STR_LEN);
    strncpy(playerArea->entities[3], router->entity_id, UUID_STR_LEN);
    strncpy(playerArea->entities[4], areaRouter->entity_id, UUID_STR_LEN);
}