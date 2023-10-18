#ifndef W0RMNET_CONNECTION_H
#define W0RMNET_CONNECTION_H

#include "utils/device_id.h"
#include "../utils/uuid.h"

typedef struct {
    char parentEntityId[UUID_STR_LEN]; // Parent device in network hierarchy
    char toEntityIds[20][UUID_STR_LEN];
    int numConns;
    int maxConns;
} Connection; // Wire-like structure

int connection_add_device(char* entityId, char* deviceId);

int connection_set_parent(char* entityId, char* parentEntityId);

void iterate_connections(void (*cb)(char*,Connection*));

char* comp_connection_serialize(Connection* connection);

Connection* comp_connection_deserialize(char* data);

#endif //W0RMNET_CONNECTION_H
