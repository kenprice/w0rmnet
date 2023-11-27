#include "world_state.h"
#include "../components/component_registry.h"

WorldState worldState;
WorldState* worldStatePtr;

void init_world_state() {
    worldState.currentArea = worldMap.playerArea;

    worldState.numExploits = 0;
    worldState.numCredDumps = 0;
    worldState.numWorms = 0;
}

/**
 * Add exploit to player's collection; skip if exists
 * @param exploit
 * @return true if added new
 */
bool world_state_add_exploit(Exploit* exploit) {
    for (int i = 0; i < worldState.numExploits; i++) {
        if (worldState.exploits[i] == exploit) return false;
    }
    worldState.exploits[worldState.numExploits++] = exploit;
    return true;
}

/**
 * Add cred dump to player's collection; skip if exists
 * @param credDump
 * @return true if added new
 */
bool world_state_add_cred_dump(CredDump* credDump) {
    for (int i = 0; i < worldState.numCredDumps; i++) {
        if (worldState.credDumps[i] == credDump) return false;
    }
    worldState.credDumps[worldState.numCredDumps++] = credDump;
    return true;
}
