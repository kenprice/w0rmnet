#include "components.h"

void init_component_registry(ComponentRegistry* registry) {
    registry->devices = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    registry->positions = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    registry->sprites = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
}

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

// System
void device_rendering_system(Texture2D texture, ComponentRegistry* registry) {
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, registry->sprites);

    guint* key_;
    Sprite* val;

    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &val))
    {
        Position* position = (Position*)g_hash_table_lookup(registry->positions, key_);
        Device* dev = (Device*)g_hash_table_lookup(registry->devices, key_);

        DrawTextureRec(texture, sprite_sheet[val->sprite_id], position->coord, WHITE);
    }
}
