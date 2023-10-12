#include "router.h"
#include "../components/component_registry.h"

Router entity_router_create_blank() {
    Router router;
    char *entity_id = generate_uuid();
    router.entity_id = entity_id;

    char *device_id = generate_device_id();
    router.device.type = DEVICE_TYPE_ROUTER;
    strncpy(router.device.entity_id, entity_id, UUID_STR_LEN);
    strncpy(router.device.id, device_id, DEVICE_ID_LEN);

    router.position.coord.x = 0;
    router.position.coord.y = 0;

    router.sprite.sprite_id = SPRITE_ROUTER;

    strncpy(router.connection.from_device_id, device_id, DEVICE_ID_LEN);
    router.connection.num_conns = 0;
    router.connection.max_conns = 100;

    strncpy(router.packet_buffer.entity_id, entity_id, UUID_STR_LEN);
    router.packet_buffer.send_q = packet_queue_alloc(50);
    router.packet_buffer.recv_q = packet_queue_alloc(50);

    memset(router.route_table.gateway, 0, DEVICE_ID_LEN);
    router.route_table.max_records = 100;
    router.route_table.num_records = 0;

    return router;
}

char* entity_router_register_components(Router router) {
    char* entity_id = router.entity_id;

    register_device(router.device, entity_id);

    Position* position = calloc(1, sizeof(Position));
    memcpy(position, &router.position, sizeof(Position));
    g_hash_table_insert(component_registry.positions, entity_id, position);

    Sprite* sprite = calloc(1, sizeof(Sprite));
    memcpy(sprite, &router.sprite, sizeof(Sprite));
    g_hash_table_insert(component_registry.sprites, entity_id, sprite);

    Connection* connection = calloc(1, sizeof(Connection)); // calloc initializes to zeroes
    memcpy(connection, &router.connection, sizeof(Connection));
    g_hash_table_insert(component_registry.connections, entity_id, connection);

    PacketBuffer* packet_buffer = calloc(1, sizeof(PacketBuffer));
    packet_buffer->send_q = packet_queue_alloc(10);
    packet_buffer->recv_q = packet_queue_alloc(10);
    strcpy(packet_buffer->entity_id, entity_id);
    g_hash_table_insert(component_registry.packet_buffers, entity_id, packet_buffer);

    RouteTable* route_table = calloc(1, sizeof(RouteTable));
    memcpy(route_table, &router.route_table, sizeof(RouteTable));
    g_hash_table_insert(component_registry.route_tables, entity_id, route_table);

    return entity_id;
}
