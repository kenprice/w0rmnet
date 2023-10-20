#ifndef W0RMNET_ROUTER_H
#define W0RMNET_ROUTER_H

#include "../components/components.h"

typedef struct {
    char entityId[UUID_STR_LEN];
    Device device;
    Position position;
    Sprite sprite;
    Connection connection;
    PacketBuffer packetBuffer;
} Router;

Router* entity_router_create_blank();

char* entity_router_register_components(Router router);

char* entity_router_serialize(Router router);

char* entity_router_serialize(Router machine);

Router entity_router_deserialize(char* data);

#endif //W0RMNET_ROUTER_H
