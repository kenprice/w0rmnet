#include "component_registry.h"

ComponentRegistry component_registry;

void init_component_registry() {
    component_registry.devices = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    component_registry.positions = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    component_registry.sprites = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    component_registry.connections = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    component_registry.packet_buffers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    component_registry.route_tables = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    component_registry.process_managers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    component_registry.proc_msg_queues = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
}
