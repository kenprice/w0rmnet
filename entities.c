#include "entities.h"
#include "utils.h"

char* create_machine_full(ComponentRegistry* registry, Machine machine) {
    char* entity_id = generate_uuid();

    Device* device = calloc(1, sizeof(Device));
    memcpy(device, &(machine.device), sizeof(Device));
    strncpy(device->entity_id, entity_id, UUID_STR_LEN);
    g_hash_table_insert(registry->devices, entity_id, device);

    Position* position = calloc(1, sizeof(Position));
    memcpy(position, &machine.position, sizeof(Position));
    g_hash_table_insert(registry->positions, entity_id, position);

    Sprite* sprite = calloc(1, sizeof(Sprite));
    memcpy(sprite, &machine.sprite, sizeof(Sprite));
    g_hash_table_insert(registry->sprites, entity_id, sprite);

    Connection* connection = calloc(1, sizeof(Connection)); // calloc initializes to zeroes
    memcpy(connection, &machine.connection, sizeof(Connection));
    g_hash_table_insert(registry->connections, entity_id, connection);

    return entity_id;
}

char* create_router_full(ComponentRegistry* registry, Router router) {
    char* entity_id = generate_uuid();

    Device* device = calloc(1, sizeof(Device));
    memcpy(device, &router.device, sizeof(Device));
    strncpy(device->entity_id, entity_id, UUID_STR_LEN);
    g_hash_table_insert(registry->devices, entity_id, device);

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

void create_entities(ComponentRegistry* registry) {
    Machine machine;
    machine.sprite.sprite_id = SPRITE_SERVER;
    strncpy(machine.device.id, "1337", DEVICE_ID_LEN);
    machine.position.coord = (Vector2){0, 0};
    strncpy(machine.connection.from_device_id, "1337", DEVICE_ID_LEN);
    strncpy(machine.connection.to_device_id, "route1000", DEVICE_ID_LEN);
    create_machine_full(registry, machine);

    Router router;
    router.sprite.sprite_id = SPRITE_ROUTER;
    strncpy(router.device.id, "route1000", DEVICE_ID_LEN);
    router.position.coord = (Vector2){3, 3};
    strncpy(router.connection.from_device_id, "route1000", DEVICE_ID_LEN);
    strncpy(router.connection.to_device_id, "1337", DEVICE_ID_LEN);
    create_router_full(registry, router);
}
