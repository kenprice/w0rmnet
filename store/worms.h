#ifndef W0RMNET_WORMS_H
#define W0RMNET_WORMS_H

#include "cred_dumps.h"
#include "exploits.h"

typedef enum {
    WormSlotLocked,
    WormSlotEmpty,
    WormSlotRemoteExploit,
    WormSlotExploitFileServer,
    WormSlotCredentialAttack,
} WormSlotType;

typedef union {
    void* pointer;
    Exploit* exploit;
    CredDump* credDump;
} WormSlotContent;

typedef struct {
    WormSlotType type;
    WormSlotContent content;
} WormSlot;

typedef struct {
    char wormName[20];
    WormSlot slots[12];
    int numSlots;
} Worm;

void worms_copy_worm_summary(Worm* worm, char* buffer);

#endif //W0RMNET_WORMS_H
