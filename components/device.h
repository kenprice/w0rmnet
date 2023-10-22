#ifndef W0RMNET_DEVICE_H
#define W0RMNET_DEVICE_H

#include "../utils/uuid.h"
#include "utils/device_id.h"

#define DEVICE_OWNER_NOBODY 0
#define DEVICE_OWNER_PLAYER 1

#define DEVICE_NOT_PWNED 0
#define DEVICE_PWNED 1

typedef enum {
    DEVICE_TYPE_GENERIC = 0,
    DEVICE_TYPE_ROUTER = 1,
} DeviceType ;

extern const char* DeviceTypeLabel[2];

typedef struct {
    char entityId[UUID_STR_LEN];
    char name[DEVICE_NAME_LEN];
    char address[100]; // FQ Adress is something like nightcity::metro:d0rkyd3vel.sn0rtt0k3r.b3efsdr34d
    DeviceType type;
    int owner; // 0 for nobody, 1 for player
    int pwned; // 0 for not pwned, 1 for pwned;
    int visible; // 0 for not visible, 1 for visible
} Device;

char* find_device_entity_id_by_device_id(char* deviceId);

Device* find_device_by_address(char* address);

Device* find_device_by_coord(int x, int y);

void register_device(Device device, char* entityId);

void iterate_devices(void (*cb)(char*,Device*));

char* comp_device_serialize(Device* device);

Device* comp_device_deserialize(char* data);

#endif //W0RMNET_DEVICE_H
