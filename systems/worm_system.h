#ifndef W0RMNET_WORM_SYSTEM_H
#define W0RMNET_WORM_SYSTEM_H

#include "../components/device.h"
#include "../store/worms.h"
#include "../utils/timer.h"

#define WORM_STATE_INACTIVE 0
#define WORM_STATE_ACTIVE 1
#define WORM_STATE_COOLDOWN 2

// State of single worm
typedef struct {
    Worm* worm;

    Device* infectedDevices[1000];
    int numInfectedDevices;
} WormState;

// State of all worms
typedef struct {
    WormState wormStates[100];
    int numWorms;
} WormSystemState;

extern WormSystemState wormSystemState;

void init_worm_system();

void update_worm_system();

#endif //W0RMNET_WORM_SYSTEM_H
