#include "glib.h"
#include "worm_system.h"
#include "../components/component_registry.h"
#include "../components/infection.h"
#include "../world/world_state.h"
#include "../lib/log/log.h"

WormSystemState wormSystemState;

#define WORM_SYSTEM_TIMER_COOLDOWN 5
#define WORM_SYSTEM_TIMER_INTERVAL 1

void worm_system_update_worm_state(WormState* wormState);

void init_worm_system() {
    log_debug("Initializing w0rm system");
    for (int i = 0; i < worldState.numWorms; i++) {
        log_debug("Initialized worm %s", worldState.worms[i].wormName);
        wormSystemState.wormStates[i].worm = &worldState.worms[i];
        wormSystemState.wormStates[i].state = WORM_STATE_INACTIVE;
        wormSystemState.wormStates[i].curActiveSlot = 0;
        wormSystemState.wormStates[i].numTargetDevices = 0;
        wormSystemState.wormStates[i].numInfectedDevices = 0;
        wormSystemState.wormStates[i].targetsInfected = 0;
        wormSystemState.wormStates[i].targetsCompleted = 0;
        StartTimer(&wormSystemState.wormStates[i].timer, WORM_SYSTEM_TIMER_INTERVAL);
    }
    wormSystemState.numWorms = worldState.numWorms;
}

void update_worm_system() {
    for (int i = 0; i < wormSystemState.numWorms; i++) {
        if (wormSystemState.wormStates[i].state == WORM_STATE_COOLDOWN) {
            wormSystemState.wormStates[i].state = WORM_STATE_ACTIVE;
            StartTimer(&wormSystemState.wormStates[i].timer, WORM_SYSTEM_TIMER_COOLDOWN);
        } else if (TimerDone(wormSystemState.wormStates[i].timer)) {
            worm_system_update_worm_state(&wormSystemState.wormStates[i]);
            StartTimer(&wormSystemState.wormStates[i].timer, WORM_SYSTEM_TIMER_INTERVAL);
        }
    }
}

void worm_system_try_remote_exploit(Exploit* exploit, Device* targetDevice) {
    ProcessManager* processManager = g_hash_table_lookup(componentRegistry.processManagers, targetDevice->entityId);
    if (!processManager) return;

    for (int i = 0; i < processManager->numProcs; i++) {
        Process proc = processManager->processes[i];
        bool nameMatch = strcmp(proc.program.name, exploit->programName) == 0;
        bool versionMatch = proc.program.version == exploit->programVersion;

        if (nameMatch == versionMatch) {
            // If target is reachable, then pwnt
        }
    }
}



// TODO: Rearrange logic like this?
// 1. For each Infected device
// 2. For each worm on infected device
// 3. If current slot is active attack
// 4.   For each nearby target      4 and 5 can be implemented using packet Q
// 5.   Do the thing                and associating packet w/ exploit

void worm_system_update_worm_state(WormState* wormState) {
    if (wormState->state == WORM_STATE_INACTIVE) return;

    bool isCurrentSlotDone = false;

    WormSlot currentWormSlot = wormState->worm->slots[wormState->curActiveSlot];
    switch (currentWormSlot.type) {
        case WormSlotRemoteExploit: {
            int targetIdx = wormState->targetsCompleted-1;
            if (wormState->targetsCompleted >= wormState->numTargetDevices) {
                isCurrentSlotDone = true;
                break;
            }
            worm_system_try_remote_exploit(currentWormSlot.content.exploit, wormState->targetDevices[targetIdx]);
            wormState->targetsCompleted++;
            break;
        }
        case WormSlotCredentialAttack:
            break;
        case WormSlotEmpty:
        default:
            isCurrentSlotDone = true;
            break;
    }

    if (isCurrentSlotDone) {
        log_debug("Worm %s finished slot %d", wormState->worm->wormName, wormState->curActiveSlot);
        wormState->curActiveSlot++;
        if (wormState->curActiveSlot >= wormState->worm->numSlots) {
            log_debug("Worm %s finished loop, cooling down", wormState->worm->wormName);
            wormState->curActiveSlot = 0;
            wormState->state = WORM_STATE_COOLDOWN;
        }
    }
}

void worm_system_add_worm(Worm worm) {

}

void worm_system_deploy_worm(int wormIdx, Device* device) {
    WormState* wormState = &wormSystemState.wormStates[wormIdx];
    int numInfected = wormState->numInfectedDevices;
    for (int i = 0; i < numInfected; i++) {
        if (wormState->infectedDevices[i] == device) {
            return;
        }
    }

    Infection* infection = g_hash_table_lookup(componentRegistry.infections, device->entityId);
    if (!infection) {
        infection = calloc(1, sizeof(Infection));
        g_hash_table_insert(componentRegistry.infections, device->entityId, infection);
        infection->numWorms = 0;
    }

    bool isNew = comp_infection_add_worm(device->entityId, wormState->worm);
    if (!isNew) {
        log_debug("Device %s is already infected with %s", device->address, wormState->worm->wormName);
        return;
    }

    log_debug("Infected %s with %s", device->address, wormState->worm->wormName);
    wormState->infectedDevices[wormState->numInfectedDevices++] = device;
    if (wormState->state == WORM_STATE_INACTIVE) {
        wormState->state = WORM_STATE_ACTIVE;
    }
}
