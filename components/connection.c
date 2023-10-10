#include <string.h>

#include "connection.h"

int add_device_to_connection(ComponentRegistry* registry, char* entity_id, char* device_id) {
    Connection* connection = (Connection*)g_hash_table_lookup(registry->connections, entity_id);
    if (connection->num_conns >= connection->max_conns) return 0;
    strcpy(connection->to_device_id[connection->num_conns], device_id);
    connection->num_conns++;
    return connection->num_conns;
}
