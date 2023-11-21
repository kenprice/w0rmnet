#ifndef W0RMNET_INFECTION_H
#define W0RMNET_INFECTION_H

#include "../store/worms.h"

typedef struct {
    Worm* worms[20];
    int numWorms;
} Infection;

bool comp_infection_add_worm(char* entityId, Worm* worm);

#endif //W0RMNET_INFECTION_H
