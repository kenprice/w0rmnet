#include "sprite.h"
#include "component_registry.h"

void iterate_sprites(void (*cb)(char*,Sprite*)) {
    GHashTableIter iter;
    char* entity_id;
    Sprite* sprite;

    g_hash_table_iter_init(&iter, component_registry.sprites);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &sprite)) {
        (*cb)(entity_id, sprite);
    }
}
