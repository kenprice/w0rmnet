#include "entities.h"
#include "../components/component_registry.h"
#include "../entities/machine.h"
#include "../entities/router.h"

void create_entities() {
    // Top-level
    Router gateway_router = entity_router_create_blank();;
    gateway_router.position.coord = (Vector2){3, 0};

    // LANs
    Router router1 = entity_router_create_blank();
    router1.position.coord = (Vector2){0, 0};

    Router router2 = entity_router_create_blank();;
    router2.position.coord = (Vector2){6, 0};

    // Machines within LANs
    Machine* machine1 = entity_machine_create_blank();
    machine1->position.coord = (Vector2){0, 3};
    entity_machine_register_components(*machine1);

    Machine* machine2 = entity_machine_create_blank();
    machine2->position.coord = (Vector2){6, 6};
    entity_machine_register_components(*machine2);

    strncpy(router1.route_table.gateway, gateway_router.device.id, DEVICE_ID_LEN);
    strncpy(router2.route_table.gateway, gateway_router.device.id, DEVICE_ID_LEN);

    entity_router_register_components(router1);
    entity_router_register_components(router2);
    entity_router_register_components(gateway_router);

    // Create test connections
    add_device_to_connection(machine1->entity_id, router1.device.id);
    add_device_to_connection(router1.entity_id, machine1->device.id);
    add_device_to_connection(machine2->entity_id, router2.device.id);
    add_device_to_connection(router2.entity_id, machine2->device.id);

    add_device_to_connection(router1.entity_id, gateway_router.device.id);
    add_device_to_connection(router2.entity_id, gateway_router.device.id);

    add_device_to_connection(gateway_router.entity_id, router1.device.id);
    add_device_to_connection(gateway_router.entity_id, router2.device.id);

    char address_buff[100] = "";
    strcat(address_buff, gateway_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router2.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine2->device.id);

    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine1->entity_id);
    packet_queue_write(&packet_buffer->send_q, packet_alloc(machine1->device.id, address_buff, "Hello!"));
    packet_queue_write(&packet_buffer->send_q, packet_alloc(machine1->device.id, address_buff, "Hello2!"));

    address_buff[0] = '\0';
    strcat(address_buff, gateway_router.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, router1.device.id);
    strcat(address_buff, ".");
    strcat(address_buff, machine1->device.id);

    packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine2->entity_id);
    packet_queue_write(&packet_buffer->send_q, packet_alloc(machine2->device.id, address_buff, "Good Bye!!"));
}
