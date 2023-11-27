#ifndef W0RMNET_WORLD_STATE_H
#define W0RMNET_WORLD_STATE_H

#include "world_map.h"
#include "../store/files.h"
#include "../store/exploits.h"
#include "../store/cred_dumps.h"
#include "../store/worms.h"

/**
 * Global game state
 */
typedef struct {
    Area* currentArea;

    // Player's stuff
    int bitCredits; // Player's bit credits
    Exploit* exploits[100];
    int numExploits;
    CredDump* credDumps[100];
    int numCredDumps;
    Worm* worms[20];
    int numWorms;
} WorldState;

extern WorldState worldState;
extern WorldState* worldStatePtr;

void init_world_state();

bool world_state_add_exploit(Exploit* exploit);

bool world_state_add_cred_dump(CredDump* credDump);

#endif //W0RMNET_WORLD_STATE_H
