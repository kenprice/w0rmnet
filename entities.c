#include "entities.h"

void create_machine(ComponentRegistry* registry, char* id, float x, float y) {
    char buffer[100] = "machine-";
    strcat(buffer, id);
    char *entity_id = strdup(buffer);

    Device* device = malloc(sizeof(Device));
    strcpy(device->id, id);
    g_hash_table_insert(registry->devices, entity_id, device);

    Position* position = malloc(sizeof(Position));
    position->coord = (Vector2){x, y};
    g_hash_table_insert(registry->positions, entity_id, position);

    Sprite* sprite = malloc(sizeof(Sprite));
    sprite->sprite_id = SPRITE_SERVER;
    g_hash_table_insert(registry->sprites, entity_id, sprite);
}

void create_router(ComponentRegistry* registry, char* id, float x, float y) {
    char buffer[100] = "router-";
    strcat(buffer, id);
    char *entity_id = strdup(buffer);

    Device* device = malloc(sizeof(Device));
    strcpy(device->id, id);
    g_hash_table_insert(registry->devices, entity_id, device);

    Position* position = malloc(sizeof(Position));
    position->coord = (Vector2){x, y};
    g_hash_table_insert(registry->positions, entity_id, position);

    Sprite* sprite = malloc(sizeof(Sprite));
    sprite->sprite_id = SPRITE_ROUTER;
    g_hash_table_insert(registry->sprites, entity_id, sprite);
}

void create_entities(ComponentRegistry* registry) {
    create_machine(registry, "1337", 3*64, 3*64);
    create_router(registry, "1081", 6*64, 3*64);
}
