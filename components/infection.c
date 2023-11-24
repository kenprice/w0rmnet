#include "infection.h"
#include "component_registry.h"

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
