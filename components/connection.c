#include <string.h>
#include "connection.h"
#include "component_registry.h"

/**
 * Add a device to a Connection. Connection is queried by entity ID.
 * @param entity_id Connection entity ID
 * @param device_id Device ID to add to Connection
 * @return Number of connections
 */
int add_device_to_connection(char* entity_id, char* device_id) {
    Connection* connection = (Connection*)g_hash_table_lookup(component_registry.connections, entity_id);
    if (connection->num_conns >= connection->max_conns) return 0;
    strcpy(connection->to_device_id[connection->num_conns], device_id);
    connection->num_conns++;
    return connection->num_conns;
}
