#ifndef W0RMNET_LOOT_H
#define W0RMNET_LOOT_H

#include "../store/exploits.h"
#include "../store/cred_dumps.h"

typedef struct {
    int bitCredits;
    Exploit* exploits[10];
    int numExploits;
    CredDump* credDumps[10];
    int numCredDumps;
} Loot;

#endif //W0RMNET_LOOT_H
