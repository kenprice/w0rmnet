#include "component_registry.h"

void init_component_registry(ComponentRegistry* registry) {
    registry->devices = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    registry->positions = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    registry->sprites = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    registry->connections = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
}
