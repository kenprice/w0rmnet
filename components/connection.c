#include <string.h>
#include "connection.h"
#include "component_registry.h"

/**
 * Add a device to a Connection. Connection is queried by entity ID.
 * @param entityId Connection entity ID of this
 * @param deviceId Entity ID of entity this is connected to
 * @return Number of connections
 */
int connection_add_device(char* entityId, char* deviceId) {
    Connection* connection = (Connection*)g_hash_table_lookup(componentRegistry.connections, entityId);
    if (connection->numConns >= connection->maxConns) return 0;
    strcpy(connection->toEntityIds[connection->numConns], deviceId);
    connection->numConns++;
    return connection->numConns;
}

int connection_set_parent(char* entityId, char* parentEntityId) {
    Connection* connection = (Connection*)g_hash_table_lookup(componentRegistry.connections, entityId);
    strcpy(connection->parentEntityId, parentEntityId);
}

void iterate_connections(void (*cb)(char*,Connection*)) {
    GHashTableIter iter;
    char* entity_id;
    Connection* connection;

    g_hash_table_iter_init(&iter, componentRegistry.connections);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &connection)) {
        (*cb)(entity_id, connection);
    }
}