#include "infection.h"
#include "component_registry.h"
#include "known_hosts.h"

/**
 * Adds worm to entity. Creates Infection component if one does not yet exist.
 *
 * @param entityId
 * @param worm
 * @return true if there are any changes
 */
bool comp_infection_add_worm(char* entityId, Worm* worm) {
    Infection* infection = g_hash_table_lookup(componentRegistry.infections, entityId);
    if (!infection) {
        infection = calloc(1, sizeof(Infection));
        g_hash_table_insert(componentRegistry.infections, entityId, infection);
    }

    // Don't add if already exists
    for (int i = 0; i < infection->numWorms; i++) {
        if (infection->worms[i] == worm) {
            return false;
        }
    }

    infection->worms[infection->numWorms++] = worm;
    return true;
}

/**
 * Increments current known host.
 *
 * @param entityId Entity ID of the Infection; same as the infected device
 * @param infection
 * @return true if end reached
 */
bool comp_infection_increment_known_host(char* entityId, Infection* infection) {
    KnownHosts* knownHosts = g_hash_table_lookup(componentRegistry.knownHosts, entityId);
    if (!infection && !knownHosts) return false;

    infection->curKnownHost++;
    if (infection->curKnownHost >= knownHosts->numEntities) {
        infection->curKnownHost = 0;
        return true;
    }
    return false;
}

/**
 * Increments active slot of current worm. Increments current worm if end of slot list reached.
 * Worms on an infected system are run one at a time.
 *
 * @param infection
 */
void comp_infection_increment_active_slot(Infection* infection) {
    if (!infection) return;

    Worm* worm = infection->worms[infection->curWorm];
    if (!worm) {
        infection->curActiveSlot = 0;
        infection->curWorm = 0;
        return;
    }

    infection->curActiveSlot++;
    if (infection->curActiveSlot >= worm->numSlots) {
        infection->curActiveSlot = 0;
        infection->curWorm++;
    }

    if (infection->curWorm >= infection->numWorms) {
        infection->curWorm = 0;
    }
}
