#include "world_state.h"

WorldState worldState;
WorldState* worldStatePtr;

void init_world_state() {
    worldState.numKnownHosts = 0;
    worldState.currentArea = worldMap.playerArea;
}
