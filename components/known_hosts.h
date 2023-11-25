#ifndef W0RMNET_KNOWN_HOSTS_H
#define W0RMNET_KNOWN_HOSTS_H

#include <stdbool.h>
#include "../utils/uuid.h"

typedef struct {
    char entities[100][UUID_STR_LEN];
    int numEntities;
} KnownHosts;

void comp_known_hosts_add_entity(KnownHosts* knownHosts, char* selfEntityId, char* entityId);

bool comp_known_hosts_entity_in_list(KnownHosts* knownHosts, char* entityId);

#endif //W0RMNET_KNOWN_HOSTS_H
