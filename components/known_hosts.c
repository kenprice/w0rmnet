#include <string.h>
#include "known_hosts.h"

/**
 * Add entity to known hosts, e.g. when receiving packet
 *
 * @param knownHosts known hosts
 * @param selfEntityId ID of KnownHost
 * @param entityId entity to add
 */
void comp_known_hosts_add_entity(KnownHosts* knownHosts, char* selfEntityId, char* entityId) {
    for (int i = 0; i < knownHosts->numEntities; i++) {
        if (strcmp(knownHosts->entities[i], entityId) == 0) return;
        if (strcmp(knownHosts->entities[i], selfEntityId) == 0) return;
    }

    strncpy(knownHosts->entities[knownHosts->numEntities++], entityId, UUID_STR_LEN);
}
