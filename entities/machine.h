#ifndef W0RMNET_MACHINE_H
#define W0RMNET_MACHINE_H

#include "../components/components.h"

typedef struct {
    char* entityId;
    Device device;
    Position position;
    Sprite sprite;
    OperatingSystem os;
    Connection connection;
    PacketBuffer packetBuffer;
} Machine;

Machine* entity_machine_create_blank();

char* entity_machine_register_components(Machine machine);

#endif //W0RMNET_MACHINE_H
