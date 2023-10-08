#include "component_registry.h"

void init_component_registry(ComponentRegistry* registry) {
    registry->devices = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    registry->positions = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    registry->sprites = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
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
