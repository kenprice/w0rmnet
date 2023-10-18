#include <stdio.h>
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

char* comp_connection_serialize(Connection* connection) {
    char entityIdsBuffer[1000] = "";
    for (int i = 0; i < connection->numConns; i++) {
        strcat(entityIdsBuffer, connection->toEntityIds[i]);
        strcat(entityIdsBuffer, ",");
    }
    entityIdsBuffer[strlen(entityIdsBuffer)-1] = '\0';

    char buffer[1000];
    sprintf(buffer, "%s;%s;%d;%d", connection->parentEntityId, entityIdsBuffer, connection->numConns, connection->maxConns);

    char* data = calloc(1, sizeof(char)*strlen(buffer)+1);
    strcpy(data, buffer);
    return data;
}

Connection* comp_connection_deserialize(char* data) {
    Connection* connection = calloc(1, sizeof(Connection));

    char** splitData = g_strsplit(data, ";", 4);

    strcpy(connection->parentEntityId, splitData[0]);
    connection->numConns = atoi(splitData[2]);
    connection->maxConns = atoi(splitData[3]);

    char** toEntityIds = g_strsplit(splitData[1], ",", connection->numConns);

    for (int i = 0; i < connection->numConns; i++) {
        strcat(connection->toEntityIds[i], toEntityIds[i]);
    }

    g_strfreev(splitData);
    g_strfreev(toEntityIds);

    return connection;
}
