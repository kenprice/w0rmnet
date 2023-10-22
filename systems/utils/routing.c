#include "routing.h"
#include "../../components/component_registry.h"
#include "../../components/components.h"

char* search_device_by_relative_address(char* entityId, char* address) {
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

    char* firstPart = path[0];
    Device* curDevice = (Device*)g_hash_table_lookup(componentRegistry.devices, entityId);

    // Find top-level
    while(strcmp(curDevice->name, firstPart) != 0) {
        Connection* connection = (Connection*)g_hash_table_lookup(componentRegistry.connections, curDevice->entityId);
        char* toEntity = connection->parentEntityId;

        Device* device = (Device*)g_hash_table_lookup(componentRegistry.devices, toEntity);
        if (device->type != DEVICE_TYPE_ROUTER) continue;

        curDevice = device;
    }

    if (parts == 1) return curDevice->entityId;

    // Find target
    Connection* connection = (Connection*)g_hash_table_lookup(componentRegistry.connections, curDevice->entityId);
    char* curPart = path[parts];
    while(parts) {
        parts--;
        for (int i = 0; i < connection->numConns; i++) {
            char* toEntity = connection->toEntityIds[i];
            Device* device = g_hash_table_lookup(componentRegistry.devices, toEntity);
            if (strcmp(curPart, device->name) != 0) continue;

            if (parts == 0) {
                return toEntity; // found
            }

            connection = (Connection*)g_hash_table_lookup(componentRegistry.connections, toEntity);
        }
        curPart = path[parts];
    }

    return NULL;
}

char* convert_full_address_to_relative_address(char* fromEntityId, char* targetAddress) {
    Device* fromDevice = (Device*)g_hash_table_lookup(componentRegistry.devices, fromEntityId);
    Device* targetDevice = find_device_by_address(targetAddress);
    if (!targetDevice) return NULL;

    char** splitOriginAddress = g_strsplit(fromDevice->address, ".", 10);
    char** splitTargetAddress = g_strsplit(targetDevice->address, ".", 10);
    char simplifiedTargetAddress[100] = "";

    int i;
    for (i = 0; i < 10; i++) {
        if (splitOriginAddress[i] == NULL || splitTargetAddress[i] == NULL) {
            break;
        }
        if (strcmp(splitOriginAddress[i], splitTargetAddress[i]) != 0) {
            break;
        }
    }
    for (int j = i-1; j < 10; j++) {
        if (splitTargetAddress[j] != NULL && strlen(splitTargetAddress[j])) {
            strcat(simplifiedTargetAddress, splitTargetAddress[j]);
            strcat(simplifiedTargetAddress, ".");
        } else {
            break;
        }
    }
    simplifiedTargetAddress[strlen(simplifiedTargetAddress)-1] = '\0';

    char* returnTargetAddress = calloc(strlen(simplifiedTargetAddress)+1, sizeof(char));
    strcpy(returnTargetAddress, simplifiedTargetAddress);
    return returnTargetAddress;
}

char* network_find_common_ancestor(char* entityId1, char* entityId2) {
    char ancestors1[20][UUID_STR_LEN];
    char numAncestors1 = 0;
    char ancestors2[20][UUID_STR_LEN];
    char numAncestors2 = 0;

    Connection* curConn = (Connection*)g_hash_table_lookup(componentRegistry.connections, entityId1);
    while (curConn) {
        char* curEntityId = curConn->parentEntityId;
        curConn = (Connection*)g_hash_table_lookup(componentRegistry.connections, curEntityId);
        if (curConn) {
            strcpy(ancestors1[numAncestors1], curEntityId);
            numAncestors1++;
        }
    }

    curConn = (Connection*)g_hash_table_lookup(componentRegistry.connections, entityId2);
    while (curConn) {
        char* curEntityId = curConn->parentEntityId;
        curConn = (Connection*)g_hash_table_lookup(componentRegistry.connections, curEntityId);
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
