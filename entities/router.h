#ifndef W0RMNET_ROUTER_H
#define W0RMNET_ROUTER_H

#include "../components/components.h"
#include "../components/component_registry.h"

typedef struct {
    Device device;
    Position position;
    Sprite sprite;
    Connection connection; // TODO: Extend to many connections
} Router;

char* create_router_full(ComponentRegistry* registry, Router router);

#endif //W0RMNET_ROUTER_H
