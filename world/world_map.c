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

    // LANs
    Router router = entity_router_create_blank();
    router.position.coord = (Vector2){7, 4};
    router.device.owner = DEVICE_OWNER_PLAYER;

    // Machines within LANs
    Machine* machine1 = entity_machine_create_blank();
    machine1->position.coord = (Vector2){2, 3};
    machine1->device.owner = DEVICE_OWNER_PLAYER;
    entity_machine_register_components(*machine1);

    Machine* machine2 = entity_machine_create_blank();
    machine2->position.coord = (Vector2){7, 6};
    entity_machine_register_components(*machine2);

    Machine* machine3 = entity_machine_create_blank();
    machine3->position.coord = (Vector2){9, 6};
    entity_machine_register_components(*machine3);

    strncpy(router.route_table.gateway, gateway_router.device.id, DEVICE_ID_LEN);

//    entity_router_register_components(router1);
    entity_router_register_components(router);
    entity_router_register_components(gateway_router);

    // Create connections
    add_device_to_connection(machine1->entity_id, gateway_router.device.id);
    add_device_to_connection(gateway_router.entity_id, machine1->device.id);
    add_device_to_connection(machine2->entity_id, router.device.id);
    add_device_to_connection(router.entity_id, machine2->device.id);
    add_device_to_connection(machine3->entity_id, router.device.id);
    add_device_to_connection(router.entity_id, machine3->device.id);

//    add_device_to_connection(router1.entity_id, gateway_router.device.id);
    add_device_to_connection(router.entity_id, gateway_router.device.id);

//    add_device_to_connection(gateway_router.entity_id, router1.device.id);
    add_device_to_connection(gateway_router.entity_id, router.device.id);

    char address_buff[100] = "";
    strcat(address_buff, gateway_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine2->device.id);

    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine1->entity_id);
//    packet_queue_write(&packet_buffer->send_q, packet_alloc(address_buff, "Ping?"));

    address_buff[0] = '\0';
    strcat(address_buff, gateway_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine1->device.id);

//    packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine2->entity_id);
//    packet_queue_write(&packet_buffer->send_q, packet_alloc(address_buff, "Ping?"));

    address_buff[0] = '\0';
    strcat(address_buff, gateway_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine3->device.id);

    packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine2->entity_id);
    packet_queue_write(&packet_buffer->send_q, packet_alloc(address_buff, "Ping?"));

    ProcessManager process_manager;
    process_manager.max_procs = 10;
    process_manager.num_procs = 1;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invokable = true;
    register_process_manager(process_manager, machine1->entity_id);

    process_manager.max_procs = 10;
    process_manager.num_procs = 1;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invokable = true;
    register_process_manager(process_manager, machine2->entity_id);

    process_manager.max_procs = 10;
    process_manager.num_procs = 1;
    process_manager.processes[0].type = PROCESS_TYPE_PING;
    process_manager.processes[0].invokable = true;
    register_process_manager(process_manager, machine3->entity_id);

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