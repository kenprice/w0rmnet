#include <string.h>
#include "connection.h"
#include "component_registry.h"

/**
 * Add a device to a Connection. Connection is queried by entity ID.
 * @param entity_id Connection entity ID
 * @param device_id Device ID to add to Connection
 * @return Number of connections
 */
int connection_add_device(char* entity_id, char* device_id) {
    Connection* connection = (Connection*)g_hash_table_lookup(component_registry.connections, entity_id);
    if (connection->num_conns >= connection->max_conns) return 0;
    strcpy(connection->to_device_id[connection->num_conns], device_id);
    connection->num_conns++;
    return connection->num_conns;
}

int connection_set_parent(char* entity_id, char* device_id) {
    Connection* connection = (Connection*)g_hash_table_lookup(component_registry.connections, entity_id);
    strcpy(connection->parent_device_id, device_id);
}

void iterate_connections(void (*cb)(char*,Connection*)) {
    GHashTableIter iter;
    char* entity_id;
    Connection* connection;

    g_hash_table_iter_init(&iter, component_registry.connections);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &connection)) {
        (*cb)(entity_id, connection);
    }
}