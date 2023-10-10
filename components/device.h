#ifndef W0RMNET_DEVICE_H
#define W0RMNET_DEVICE_H

#include "component_registry.h"
#include "../utils/uuid.h"
#include "utils/device_id.h"

typedef struct {
    char entity_id[UUID_STR_LEN];
    char id[DEVICE_ID_LEN];
} Device;

char* find_device_entity_id_by_device_id(ComponentRegistry* registry, char* device_id);

void register_device(ComponentRegistry* registry, Device device, char* entity_id);

#endif //W0RMNET_DEVICE_H
