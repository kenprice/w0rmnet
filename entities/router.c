#include "router.h"

char* create_router_full(ComponentRegistry* registry, Router router) {
    char* entity_id = generate_uuid();

    register_device(registry, router.device, entity_id);

    Position* position = calloc(1, sizeof(Position));
    memcpy(position, &router.position, sizeof(Position));
    g_hash_table_insert(registry->positions, entity_id, position);

    Sprite* sprite = calloc(1, sizeof(Sprite));
    memcpy(sprite, &router.sprite, sizeof(Sprite));
    g_hash_table_insert(registry->sprites, entity_id, sprite);

    Connection* connection = calloc(1, sizeof(Connection)); // calloc initializes to zeroes
    memcpy(connection, &router.connection, sizeof(Connection));
    g_hash_table_insert(registry->connections, entity_id, connection);

    return entity_id;
}
