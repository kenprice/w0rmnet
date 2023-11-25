#ifndef W0RMNET_INFECTION_H
#define W0RMNET_INFECTION_H

#include "../components/device.h"
#include "../store/worms.h"
#include "../utils/timer.h"

typedef struct {
    int numWorms;
    Worm* worms[20];

    // State of instance of worm(s) running on infected device
    int curWorm;
    int curActiveSlot;
    int curKnownHost; // If active slot targets a device, denotes current targeted device (from knownHosts)
    int curCred; // If active slot is credential attack, denotes current cred
    Timer runnerTimer; // Timer for the worms doing their thing. Running multiple worms do not confer perf benefit
} Infection;

bool comp_infection_add_worm(char* entityId, Worm* worm);

bool comp_infection_increment_known_host(char* entityId, Infection* infection);

void comp_infection_increment_active_slot(Infection* infection);

#endif //W0RMNET_INFECTION_H
