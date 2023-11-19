#include "world_state.h"
#include "../components/component_registry.h"

WorldState worldState;
WorldState* worldStatePtr;

void init_world_state() {
    worldState.currentArea = worldMap.playerArea;

    worldState.numExploits = 2;
    worldState.exploits[0] = ExploitsList[0];
    worldState.exploits[1] = ExploitsList[1];

    worldState.numCredDumps = 1;
    worldState.credDumps[0] = CredDumpsList[0];
}

void update_world_state() {

}