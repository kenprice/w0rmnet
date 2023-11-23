#include "component_registry.h"

ComponentRegistry componentRegistry;

void init_component_registry() {
    componentRegistry.devices = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.positions = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.sprites = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.packetBuffers = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.routeTable = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.processManagers = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.procMsgQueues = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.wires = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.polygons = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.logger = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.loots = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.infections = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
    componentRegistry.knownHosts = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, free);
}
