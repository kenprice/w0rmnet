#ifndef W0RMNET_KNOWN_HOSTS_H
#define W0RMNET_KNOWN_HOSTS_H

#include "../utils/uuid.h"

typedef struct {
    char entities[UUID_STR_LEN][100];
    int numEntities;
} KnownHosts;

void comp_known_hosts_add_entity(KnownHosts* knownHosts, char* selfEntityId, char* entityId);

#endif //W0RMNET_KNOWN_HOSTS_H
