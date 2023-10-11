#include "entities.h"
#include "../components/component_registry.h"
#include "../entities/machine.h"
#include "../entities/router.h"

void create_entities() {
    Machine machine1;
    char* machine_id1 = generate_device_id();
    machine1.sprite.sprite_id = SPRITE_SERVER;
    strncpy(machine1.device.id, machine_id1, DEVICE_ID_LEN);
    machine1.device.type = DEVICE_TYPE_GENERIC;
    machine1.position.coord = (Vector2){0, 3};
    strncpy(machine1.connection.from_device_id, machine_id1, DEVICE_ID_LEN);
    machine1.connection.max_conns = 1;
    machine1.connection.num_conns = 0;
    char* machine1_eid = create_machine_full(machine1);

    Machine machine2;
    char* machine_id2 = generate_device_id();
    machine2.sprite.sprite_id = SPRITE_SERVER;
    strncpy(machine2.device.id, machine_id2, DEVICE_ID_LEN);
    machine2.device.type = DEVICE_TYPE_GENERIC;
    machine2.position.coord = (Vector2){6, 6};
    strncpy(machine2.connection.from_device_id, machine_id2, DEVICE_ID_LEN);
    machine2.connection.max_conns = 1;
    machine2.connection.num_conns = 0;
    char* machine2_eid = create_machine_full(machine2);

    Router router;
    char* router_id1 = generate_device_id();
    router.sprite.sprite_id = SPRITE_ROUTER;
    strncpy(router.device.id, router_id1, DEVICE_ID_LEN);
    router.device.type = DEVICE_TYPE_ROUTER;
    router.position.coord = (Vector2){0, 0};
    strncpy(router.connection.from_device_id, router_id1, DEVICE_ID_LEN);
    router.connection.max_conns = 10;
    router.connection.num_conns = 0;
    char* router1_eid = create_router_full(router);

    Router router2;
    char* router_id2 = generate_device_id();
    router2.sprite.sprite_id = SPRITE_ROUTER;
    strncpy(router2.device.id, router_id2, DEVICE_ID_LEN);
    router2.device.type = DEVICE_TYPE_ROUTER;
    router2.position.coord = (Vector2){6, 0};
    strncpy(router2.connection.from_device_id, router_id2, DEVICE_ID_LEN);
    router2.connection.max_conns = 10;
    router2.connection.num_conns = 0;
    char* router2_eid = create_router_full(router2);

    Router router3;
    char* router_id3 = generate_device_id();
    router3.sprite.sprite_id = SPRITE_ROUTER;
    strncpy(router3.device.id, router_id3, DEVICE_ID_LEN);
    router3.device.type = DEVICE_TYPE_ROUTER;
    router3.position.coord = (Vector2){3, 0};
    strncpy(router3.connection.from_device_id, router_id3, DEVICE_ID_LEN);
    router3.connection.max_conns = 10;
    router3.connection.num_conns = 0;
    char* router3_eid = create_router_full(router3);

    // Create test connections
    add_device_to_connection(machine1_eid, router_id1);
    add_device_to_connection(router1_eid, machine_id1);
    add_device_to_connection(machine2_eid, router_id2);
    add_device_to_connection(router2_eid, machine_id2);

    add_device_to_connection(router1_eid, router_id3);
    add_device_to_connection(router2_eid, router_id3);

    add_device_to_connection(router3_eid, router_id1);
    add_device_to_connection(router3_eid, router_id2);

    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine1_eid);
    packet_queue_write(&packet_buffer->send_q, packet_alloc(machine_id1, machine_id2, "Hello!"));
    packet_queue_write(&packet_buffer->send_q, packet_alloc(machine_id1, machine_id2, "Hello2!"));
    packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine2_eid);
    packet_queue_write(&packet_buffer->send_q, packet_alloc(machine_id2, machine_id1, "Good Bye!!"));
//    packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, machine2_eid);
//    packet_queue_write(packet_buffer->send_q, packet_alloc(machine_id2, machine_id1, "Good day!!"));

//    packet_queue_write();
}
