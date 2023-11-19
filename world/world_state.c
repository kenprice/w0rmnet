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

    worldState.numWorms = 1;
    strcpy(worldState.worms[0].wormName, "myw0rm");
    worldState.worms[0].numSlots = 2;
    worldState.worms[0].slots[0].type = WormSlotRemoteExploit;
    worldState.worms[0].slots[0].content.exploit = &ExploitsList[0];
    worldState.worms[0].slots[1].type = WormSlotRemoteExploit;
    worldState.worms[0].slots[1].content.exploit = &ExploitsList[1];
}

void update_world_state() {

}