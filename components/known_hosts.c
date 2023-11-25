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
    if (comp_known_hosts_entity_in_list(knownHosts, entityId)) return;
    if (comp_known_hosts_entity_in_list(knownHosts, selfEntityId)) return;

    strncpy(knownHosts->entities[knownHosts->numEntities++], entityId, UUID_STR_LEN);
}

bool comp_known_hosts_entity_in_list(KnownHosts* knownHosts, char* entityId) {
    for (int i = 0; i < knownHosts->numEntities; i++) {
        if (strcmp(knownHosts->entities[i], entityId) == 0) return true;
    }
    return false;
}

