#ifndef W0RMNET_ROUTER_H
#define W0RMNET_ROUTER_H

#include "../components/components.h"

typedef struct {
    char* entityId;
    Device device;
    Position position;
    Sprite sprite;
    Connection connection;
    PacketBuffer packetBuffer;
} Router;

Router* entity_router_create_blank();

char* entity_router_register_components(Router router);

char* entity_router_serialize(Router router);

#endif //W0RMNET_ROUTER_H
