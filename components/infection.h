#ifndef W0RMNET_INFECTION_H
#define W0RMNET_INFECTION_H

#include "../components/device.h"
#include "../store/worms.h"
#include "../utils/timer.h"

typedef struct {
    int numWorms;
    Worm* worms[20];
    int activeSlots[20];

    Device* infectedDevices[100];
    int numInfectedDevices;
    Device* targetDevices[100];
    int numTargetDevices;
    int numTargetsCompleted;
} Infection;

bool comp_infection_add_worm(char* entityId, Worm* worm);

#endif //W0RMNET_INFECTION_H
