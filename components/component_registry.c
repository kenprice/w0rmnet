#include "component_registry.h"

ComponentRegistry componentRegistry;

void init_component_registry() {
    componentRegistry.devices = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    componentRegistry.positions = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    componentRegistry.sprites = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    componentRegistry.connections = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    componentRegistry.packetBuffers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    componentRegistry.routeTable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    componentRegistry.processManagers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    componentRegistry.procMsgQueues = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    componentRegistry.wires = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
}
