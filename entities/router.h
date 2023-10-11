#ifndef W0RMNET_ROUTER_H
#define W0RMNET_ROUTER_H

#include "../components/components.h"

typedef struct {
    Device device;
    Position position;
    Sprite sprite;
    Connection connection;
    PacketBuffer packet_buffer;
} Router;

char* create_router_full(Router router);

#endif //W0RMNET_ROUTER_H
