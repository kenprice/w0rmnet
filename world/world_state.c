#include "world_state.h"
#include "../components/component_registry.h"

WorldState worldState;
WorldState* worldStatePtr;

void init_world_state() {
    worldState.numKnownHosts = 0;
    worldState.currentArea = worldMap.playerArea;
}

void update_world_state() {

}