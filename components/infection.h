#ifndef W0RMNET_INFECTION_H
#define W0RMNET_INFECTION_H

#include "../components/device.h"
#include "../store/worms.h"
#include "../utils/timer.h"

typedef struct {
    int numWorms;
    Worm* worms[20];

    int activeSlots[20]; // One-to-one w/ worms, the worm's active slot
    int curCred; // If active slot is credential attack, denotes current cred

    Timer runnerTimer; // Timer for the worms doing their thing. Running multiple worms do not confer perf benefit
} Infection;

bool comp_infection_add_worm(char* entityId, Worm* worm);

#endif //W0RMNET_INFECTION_H
