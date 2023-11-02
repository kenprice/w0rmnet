#ifndef W0RMNET_NETWORK_SWITCH_H
#define W0RMNET_NETWORK_SWITCH_H

#include "../components/components.h"

typedef struct {
    char entityId[UUID_STR_LEN];
    Device device;
    Position position;
    Sprite sprite;
    Polygon polygon;
    Wire wire;
    ProcessManager processManager;
    ProcMessageQueue procMessageQueue;
} NetworkSwitch;

NetworkSwitch* entity_network_switch_create_blank();

char* entity_network_switch_register_components(NetworkSwitch networkSwitch);

#endif //W0RMNET_NETWORK_SWITCH_H
