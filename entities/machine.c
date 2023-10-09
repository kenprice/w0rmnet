#include "machine.h"

char* create_machine_full(ComponentRegistry* registry, Machine machine) {
    char* entity_id = generate_uuid();

    register_device(registry, machine.device, entity_id);

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
