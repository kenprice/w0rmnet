#include "routing.h"
#include "../../components/component_registry.h"
#include "../../components/components.h"

char* search_device_by_address(char* entity_id, char* address) {
    char buffer[200];
    strcpy(buffer, address);
    char* token = strtok(buffer, ".");
    char path[10][11];
    int parts = 0;
    while (token != NULL) {
        strcpy(path[parts], token);
        token = strtok(NULL, ".");
        parts++;
    }

    char* first_part = path[0];
    Device* cur_device = (Device*)g_hash_table_lookup(component_registry.devices, entity_id);

    // Find top-level
    while(strcmp(cur_device->id, first_part) != 0) {
        Connection* connection = (Connection*)g_hash_table_lookup(component_registry.connections, cur_device->entity_id);
        char* to_entity = find_device_entity_id_by_device_id(connection->parent_device_id);

        Device* device = (Device*)g_hash_table_lookup(component_registry.devices, to_entity);
        if (device->type != DEVICE_TYPE_ROUTER) continue;

        cur_device = device;
    }

    if (parts == 1) return cur_device->entity_id;

    // Find target
    Connection* connection = (Connection*)g_hash_table_lookup(component_registry.connections, cur_device->entity_id);
    char* cur_part = path[parts];
    while(parts) {
        parts--;
        for (int i = 0; i < connection->num_conns; i++) {
            char* to_entity = find_device_entity_id_by_device_id(connection->to_device_id[i]);
            if (strcmp(cur_part, connection->to_device_id[i]) != 0) continue;

            if (parts == 0) {
                return to_entity; // found
            }

            connection = (Connection*)g_hash_table_lookup(component_registry.connections, to_entity);
        }
        cur_part = path[parts];
    }

    return NULL;
}

char* network_find_common_ancestor(char* entityId1, char* entityId2) {
    char ancestors1[20][UUID_STR_LEN];
    char numAncestors1 = 0;
    char ancestors2[20][UUID_STR_LEN];
    char numAncestors2 = 0;

    Connection* curConn = (Connection*)g_hash_table_lookup(component_registry.connections, entityId1);
    while (curConn) {
        char* curEntityId = find_device_entity_id_by_device_id(curConn->parent_device_id);
        curConn = (Connection*)g_hash_table_lookup(component_registry.connections, curEntityId);
        if (curConn) {
            strcpy(ancestors1[numAncestors1], curEntityId);
            numAncestors1++;
        }
    }

    curConn = (Connection*)g_hash_table_lookup(component_registry.connections, entityId2);
    while (curConn) {
        char* curEntityId = find_device_entity_id_by_device_id(curConn->parent_device_id);
        curConn = (Connection*)g_hash_table_lookup(component_registry.connections, curEntityId);
        if (curConn) {
            strcpy(ancestors2[numAncestors2], curEntityId);
            numAncestors2++;
        }
    }

    for (int i = 0; i < numAncestors1; i++) {
        for (int j = 0; j < numAncestors2; j++) {
            if (strcmp(ancestors1[i], ancestors2[j]) == 0) {
                // Found!
                return ancestors1[i];
            }
        }
    }

    return NULL;
}
