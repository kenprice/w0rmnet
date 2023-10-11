#ifndef W0RMNET_DEVICE_H
#define W0RMNET_DEVICE_H

#include "../utils/uuid.h"
#include "utils/device_id.h"

typedef enum {
    DEVICE_TYPE_GENERIC,
    DEVICE_TYPE_ROUTER,
} DeviceType ;

typedef struct {
    char entity_id[UUID_STR_LEN];
    char id[DEVICE_ID_LEN];
    DeviceType type;
} Device;

char* find_device_entity_id_by_device_id(char* device_id);

Device* find_device_by_coord(int x, int y);

void register_device(Device device, char* entity_id);

void iterate_devices(void (*cb)(char*,Device*));

#endif //W0RMNET_DEVICE_H
