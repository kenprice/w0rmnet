#ifndef W0RMNET_MACHINE_H
#define W0RMNET_MACHINE_H

#include "../components/components.h"

typedef struct {
    char entityId[UUID_STR_LEN];
    Device device;
    Position position;
    Sprite sprite;
    Connection connection;
    PacketBuffer packetBuffer;
    ProcessManager processManager;
    ProcMessageQueue procMessageQueue;
} Machine;

Machine* entity_machine_create_blank();

char* entity_machine_register_components(Machine machine);

char* entity_machine_serialize(Machine machine);

Machine entity_machine_deserialize(char* data);

#endif //W0RMNET_MACHINE_H
