#ifndef W0RMNET_WORLD_STATE_H
#define W0RMNET_WORLD_STATE_H

#include "world_map.h"
#include "../store/files.h"
#include "../store/exploits.h"
#include "../store/cred_dumps.h"

/**
 * Global game state
 */
typedef struct {
    Area* currentArea;

    // Player's stuff
    Exploit exploits[100];
    int numExploits;
    CredDump credDumps[100];
    int numCredDumps;
    int bitCredits; // Player's bit credits
} WorldState;

extern WorldState worldState;
extern WorldState* worldStatePtr;

void init_world_state();

void update_world_state();

#endif //W0RMNET_WORLD_STATE_H
