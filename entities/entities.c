#include "entities.h"
#include "../entities/machine.h"
#include "../entities/router.h"

void create_entities(ComponentRegistry* registry) {
    Machine machine1;
    char* machine_id1 = generate_device_id();
    machine1.sprite.sprite_id = SPRITE_SERVER;
    strncpy(machine1.device.id, machine_id1, DEVICE_ID_LEN);
    machine1.device.type = DEVICE_TYPE_GENERIC;
    machine1.position.coord = (Vector2){0, 0};
    strncpy(machine1.connection.from_device_id, machine_id1, DEVICE_ID_LEN);
    machine1.connection.max_conns = 1;
    machine1.connection.num_conns = 0;
    char* entity1 = create_machine_full(registry, machine1);

    Machine machine2;
    char* machine_id2 = generate_device_id();
    machine2.sprite.sprite_id = SPRITE_SERVER;
    strncpy(machine2.device.id, machine_id2, DEVICE_ID_LEN);
    machine2.device.type = DEVICE_TYPE_GENERIC;
    machine2.position.coord = (Vector2){1, 5};
    strncpy(machine2.connection.from_device_id, machine_id2, DEVICE_ID_LEN);
    machine2.connection.max_conns = 1;
    machine2.connection.num_conns = 0;
    char* entity2 = create_machine_full(registry, machine2);

    Router router;
    char* router_id1 = generate_device_id();
    router.sprite.sprite_id = SPRITE_ROUTER;
    strncpy(router.device.id, router_id1, DEVICE_ID_LEN);
    router.device.type = DEVICE_TYPE_ROUTER;
    router.position.coord = (Vector2){3, 3};
    strncpy(router.connection.from_device_id, router_id1, DEVICE_ID_LEN);
    router.connection.max_conns = 10;
    router.connection.num_conns = 0;
    char* entity3 = create_router_full(registry, router);

    // Create test connections
    add_device_to_connection(registry, entity1, router_id1);
    add_device_to_connection(registry, entity2, router_id1);
    add_device_to_connection(registry, entity3, machine_id1);
    add_device_to_connection(registry, entity3, machine_id2);

    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(registry->packet_buffers, entity1);
    packet_queue_write(&packet_buffer->send_q, packet_alloc(machine_id1, machine_id2, "Hello!"));
    packet_queue_write(&packet_buffer->send_q, packet_alloc(machine_id1, machine_id2, "Hello2!"));
    packet_buffer = (PacketBuffer*)g_hash_table_lookup(registry->packet_buffers, entity2);
    packet_queue_write(&packet_buffer->send_q, packet_alloc(machine_id2, machine_id1, "Good Bye!!"));
//    packet_buffer = (PacketBuffer*)g_hash_table_lookup(registry->packet_buffers, entity2);
//    packet_queue_write(packet_buffer->send_q, packet_alloc(machine_id2, machine_id1, "Good day!!"));

//    packet_queue_write();
}
