#include <stdio.h>
#include <glib.h>
#include "sprite.h"
#include "component_registry.h"

void iterate_sprites(void (*cb)(char*,Sprite*)) {
    GHashTableIter iter;
    char* entity_id;
    Sprite* sprite;

    g_hash_table_iter_init(&iter, componentRegistry.sprites);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &sprite)) {
        (*cb)(entity_id, sprite);
    }
}

char* comp_sprite_serialize(Sprite* sprite) {
    char buffer[10];

    sprintf(buffer, "%d", sprite->spriteId);

    char* data = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(data, buffer);

    return data;
}

Sprite* comp_sprite_deserialize(char* data) {
    Sprite* sprite = calloc(1, sizeof(Sprite));

    char** splitData = g_strsplit(data, ";", 1);

    sprite->spriteId = atoi(splitData[0]);

    g_strfreev(splitData);

    return sprite;
}
