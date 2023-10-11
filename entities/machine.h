#ifndef W0RMNET_MACHINE_H
#define W0RMNET_MACHINE_H

#include "../components/components.h"

typedef struct {
    Device device;
    Position position;
    Sprite sprite;
    OperatingSystem os;
    Connection connection;
    PacketBuffer packet_buffer;
} Machine;

char* create_machine_full(Machine machine);

#endif //W0RMNET_MACHINE_H
