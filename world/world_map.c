#include "world_map.h"
#include "../components/component_registry.h"
#include "../entities/machine.h"
#include "../entities/router.h"

Area player_area;
Area area_registry[100];
int area_current = 0;

void init_player_area() {
    // Top-level
    Router area_router = entity_router_create_blank();
    area_router.position.coord = (Vector2){4, 1};
    area_router.device.owner = DEVICE_OWNER_PLAYER;
    area_router.device.visible = 1;

    // LANs
    Router router = entity_router_create_blank();
    router.position.coord = (Vector2){7, 4};
    router.device.owner = DEVICE_OWNER_PLAYER;
    router.device.visible = 1;

    // Machines within LANs
    Machine* machine1 = entity_machine_create_blank();
    machine1->position.coord = (Vector2){2, 3};
    machine1->device.owner = DEVICE_OWNER_PLAYER;
    machine1->device.visible = 1;
    entity_machine_register_components(*machine1);

    Machine* machine2 = entity_machine_create_blank();
    machine2->position.coord = (Vector2){7, 6};
    entity_machine_register_components(*machine2);

    Machine* machine3 = entity_machine_create_blank();
    machine3->position.coord = (Vector2){9, 6};
    entity_machine_register_components(*machine3);

    entity_router_register_components(router);
    entity_router_register_components(area_router);

    // Create connections
    connection_add_device(machine1->entity_id, area_router.device.id);
    connection_add_device(area_router.entity_id, machine1->device.id);
    connection_add_device(machine2->entity_id, router.device.id);
    connection_add_device(router.entity_id, machine2->device.id);
    connection_add_device(machine3->entity_id, router.device.id);
    connection_add_device(router.entity_id, machine3->device.id);
    connection_add_device(router.entity_id, area_router.device.id);
    connection_add_device(area_router.entity_id, router.device.id);

    connection_set_parent(machine1->entity_id, area_router.device.id);
    connection_set_parent(machine2->entity_id, router.device.id);
    connection_set_parent(machine3->entity_id, router.device.id);
    connection_set_parent(router.entity_id, area_router.device.id);

    char address_buff[100] = "";
    strcat(address_buff, area_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine2->device.id);

    address_buff[0] = '\0';
    strcat(address_buff, area_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine1->device.id);

    address_buff[0] = '\0';
    strcat(address_buff, area_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine3->device.id);

//    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine2->entity_id);
//    packet_queue_write(&packet_buffer->send_q, packet_alloc(address_buff, "Ping?"));

    ProcessManager process_manager;
    process_manager.max_procs = 10;
    process_manager.num_procs = 2;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invocable = true;
    memset(process_manager.processes[0].state, '\0', PROCESS_STATE_LEN);
    process_manager.processes[1].type = PROCESS_TYPE_SCAN;
    process_manager.processes[1].invocable = true;
    memset(process_manager.processes[1].state, '\0', PROCESS_STATE_LEN);
    register_process_manager(process_manager, machine1->entity_id);
    ProcMessageQueue machine1_pmq = proc_msg_queue_alloc(10);
    register_proc_msg_queue(machine1_pmq, machine1->entity_id);
    address_buff[0] = '\0';
    strcat(address_buff, area_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router.device.id);
    ProcMessage* msg = proc_msg_alloc(1, address_buff);
    proc_msg_queue_write(g_hash_table_lookup(component_registry.proc_msg_queues, machine1->entity_id), msg);

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
    process_manager.num_procs = 1;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invocable = true;
    memset(process_manager.processes[0].state, '\0', PROCESS_STATE_LEN);
    register_process_manager(process_manager, machine3->entity_id);
    register_proc_msg_queue(proc_msg_queue_alloc(10), machine3->entity_id);

    // Area info
    player_area.height = 12;
    player_area.width = 12;
    player_area.num_entities = 5;
    strncpy(player_area.entities[0], machine1->entity_id, UUID_STR_LEN);
    strncpy(player_area.entities[1], machine2->entity_id, UUID_STR_LEN);
    strncpy(player_area.entities[2], machine3->entity_id, UUID_STR_LEN);
    strncpy(player_area.entities[3], router.entity_id, UUID_STR_LEN);
    strncpy(player_area.entities[4], area_router.entity_id, UUID_STR_LEN);
    area_current = 0;
    area_registry[area_current] = player_area;
}