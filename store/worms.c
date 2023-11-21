#include <stdio.h>
#include <string.h>
#include "exploits.h"
#include "worms.h"

void worms_copy_worm_summary(Worm worm, char* buffer) {
    char buf[1000] = "";
    char summary[100] = "";

    int exploitCount = 0;
    int credAttackCount = 0;

    for (int i = 0; i < worm.numSlots; i++) {
        switch(worm.slots[i].type) {
            case WormSlotRemoteExploit: {
                exploitCount++;
                break;
            }
            case WormSlotCredentialAttack: {
                credAttackCount++;
                break;
            }
        }
    }

    if (exploitCount) {
        sprintf(summary, "%d exploits", exploitCount);
        strcat(buf, summary);
    }
    if (credAttackCount) {
        sprintf(summary, "%s%d cred lists", strlen(buf) > 1 ? ", " : "", credAttackCount);
        strcat(buf, summary);
    }
    strncat(buffer, buf, 99);
}
