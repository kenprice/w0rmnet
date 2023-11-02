#ifndef W0RMNET_BOTNET_SYSTEM_H
#define W0RMNET_BOTNET_SYSTEM_H

#include "../components/components.h"
#include "../utils/timer.h"

#define BOTNET_STATE_STANDBY 0
#define BOTNET_STATE_ACTIVE 1

typedef struct {
    int state;
    Timer timer;

    Device* ownedDevices[1000];
    int numOwnedDevices;
    Device* targetDevices[1000];
    int numTargetDevices;

    int targetsCompleted;
} BotnetState;

void update_botnet_system();

void botnet_system_test_launch_login_attack();


#endif //W0RMNET_BOTNET_SYSTEM_H
