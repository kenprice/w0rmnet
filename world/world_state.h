#ifndef W0RMNET_WORLD_STATE_H
#define W0RMNET_WORLD_STATE_H

#include "world_map.h"

/**
 * Global game state
 */
typedef struct {
    char knownHosts[100][110]; // Known good registry of addresses, form perspective of player
    int numKnownHosts;

    Area* currentArea;
} WorldState;

extern WorldState worldState;
extern WorldState* worldStatePtr;

void init_world_state();

void update_world_state();

#endif //W0RMNET_WORLD_STATE_H
