#include "world_map.h"
#include "../components/component_registry.h"
#include "../entities/machine.h"
#include "../entities/router.h"

Area player_area;
Area area_registry[100];
int area_current = 0;

void init_player_area() {
    // Top-level
    Router gateway_router = entity_router_create_blank();
    gateway_router.position.coord = (Vector2){4, 1};
    gateway_router.device.owner = DEVICE_OWNER_PLAYER;
    gateway_router.device.visible = 1;

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

    strncpy(router.route_table.gateway, gateway_router.device.id, DEVICE_ID_LEN);

    entity_router_register_components(router);
    entity_router_register_components(gateway_router);

    // Create connections
    add_device_to_connection(machine1->entity_id, gateway_router.device.id);
    add_device_to_connection(gateway_router.entity_id, machine1->device.id);
    add_device_to_connection(machine2->entity_id, router.device.id);
    add_device_to_connection(router.entity_id, machine2->device.id);
    add_device_to_connection(machine3->entity_id, router.device.id);
    add_device_to_connection(router.entity_id, machine3->device.id);

    add_device_to_connection(router.entity_id, gateway_router.device.id);

    add_device_to_connection(gateway_router.entity_id, router.device.id);

    char address_buff[100] = "";
    strcat(address_buff, gateway_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine2->device.id);

    address_buff[0] = '\0';
    strcat(address_buff, gateway_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine1->device.id);

    address_buff[0] = '\0';
    strcat(address_buff, gateway_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine3->device.id);

//    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine2->entity_id);
//    packet_queue_write(&packet_buffer->send_q, packet_alloc(address_buff, "Ping?"));

    ProcessManager process_manager;
    process_manager.max_procs = 10;
    process_manager.num_procs = 1;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invokable = true;
    process_manager.processes[0].args[0] = '\0';
    memset(process_manager.processes[0].state, '\0', PROCESS_STATE_LEN);
    register_process_manager(process_manager, machine1->entity_id);
    ProcMessageQueue machine1_pmq = proc_msg_queue_alloc(10);
    register_proc_msg_queue(machine1_pmq, machine1->entity_id);
    ProcMessage* msg = proc_msg_alloc(0, address_buff);
    proc_msg_queue_write(g_hash_table_lookup(component_registry.proc_msg_queues, machine1->entity_id), msg);

    process_manager.max_procs = 10;
    process_manager.num_procs = 1;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invokable = true;
    process_manager.processes[0].args[0] = '\0';
    memset(process_manager.processes[0].state, '\0', PROCESS_STATE_LEN);
    process_manager.processes[0].state[0] = 1;
    register_process_manager(process_manager, machine2->entity_id);
    ProcMessageQueue machine2_pmq = proc_msg_queue_alloc(10);
    register_proc_msg_queue(machine2_pmq, machine2->entity_id);
    msg = proc_msg_alloc(0, address_buff);
    proc_msg_queue_write(g_hash_table_lookup(component_registry.proc_msg_queues, machine2->entity_id), msg);

    process_manager.max_procs = 10;
    process_manager.num_procs = 1;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invokable = true;
    process_manager.processes[0].args[0] = '\0';
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
    strncpy(player_area.entities[4], gateway_router.entity_id, UUID_STR_LEN);
    area_current = 0;
    area_registry[area_current] = player_area;
}