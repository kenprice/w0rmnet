#include "world_state.h"
#include "../components/component_registry.h"

WorldState worldState;
WorldState* worldStatePtr;

void init_world_state() {
    worldState.currentArea = worldMap.playerArea;

    worldState.numExploits = 1;
    worldState.exploits[0] = ExploitsList[0];
}

void update_world_state() {

}