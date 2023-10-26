#include "routing.h"
#include "../../components/component_registry.h"
#include "../../components/components.h"

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
