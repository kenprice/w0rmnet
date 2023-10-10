#ifndef W0RMNET_MACHINE_H
#define W0RMNET_MACHINE_H

#include "../components/components.h"
#include "../components/component_registry.h"

typedef struct {
    Device device;
    Position position;
    Sprite sprite;
    OperatingSystem os;
    Connection connection;
    PacketBuffer packet_buffer;
} Machine;

char* create_machine_full(ComponentRegistry* registry, Machine machine);

#endif //W0RMNET_MACHINE_H
