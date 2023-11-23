#ifndef W0RMNET_MACHINE_H
#define W0RMNET_MACHINE_H

#include "../components/device.h"
#include "../components/known_hosts.h"
#include "../components/packet.h"
#include "../components/position.h"
#include "../components/process.h"
#include "../components/route_table.h"
#include "../components/sprite.h"

typedef struct {
    char entityId[UUID_STR_LEN];
    Device device;
    KnownHosts knownHosts;
    Position position;
    Sprite sprite;
    PacketBuffer packetBuffer;
    RouteTable routeTable;
    ProcessManager processManager;
    ProcMessageQueue procMessageQueue;
} Machine;

Machine* entity_machine_create_blank();

char* entity_machine_register_components(Machine* machine);

char* entity_machine_serialize(Machine machine);

Machine entity_machine_deserialize(char* data);

#endif //W0RMNET_MACHINE_H
