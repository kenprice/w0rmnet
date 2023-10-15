#include "routing.h"
#include "../../components/component_registry.h"
#include "../../components/components.h"

/**
 * @param entity_id From which device (addresses are relative to a device)
 * @param address
 * @return entity_id of device, if found, else NULL
 */
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

    // Find target
    parts--;
    Connection* connection = (Connection*)g_hash_table_lookup(component_registry.connections, cur_device->entity_id);
    char* cur_part = path[parts];
    while(parts) {
        for (int i = 0; i < connection->num_conns; i++) {
            char* to_entity = find_device_entity_id_by_device_id(connection->to_device_id[i]);
            if (strcmp(cur_part, connection->to_device_id[i]) != 0) continue;

            parts--;
            if (parts == 0) {
                return to_entity; // found
            }

            connection = (Connection*)g_hash_table_lookup(component_registry.connections, to_entity);
        }
        cur_part = path[parts];
    }

    return NULL;
}
