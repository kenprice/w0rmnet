#include "glib.h"
#include "worm_system.h"
#include "../components/component_registry.h"
#include "../components/device.h"
#include "../components/infection.h"
#include "../components/known_hosts.h"
#include "../entities/helpers.h"
#include "../events/worm_events.h"
#include "../lib/log/log.h"
#include "../store/exploits.h"
#include "../world/world_state.h"

WormSystemState wormSystemState;

#define WORM_SYSTEM_TIMER_COOLDOWN 5
#define WORM_SYSTEM_TIMER_INTERVAL 1

void worm_system_update_worm_state(WormState* wormState);
void worm_system_update_infection(char* entityId, Infection* infection);
void worm_system_add_worm(WormEvent event);

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

    events_register_worm_event_listener(worm_system_add_worm);
}

void update_worm_system() {
    GHashTableIter iter;
    char* entityId;
    Infection* infection;

    g_hash_table_iter_init(&iter, componentRegistry.infections);
    while (g_hash_table_iter_next(&iter, (gpointer) &entityId, (gpointer) &infection)) {
        worm_system_update_infection(entityId, infection);
    }
}

void worm_system_try_remote_exploit(Device* fromDevice, Device* targetDevice, Exploit* exploit) {
    Packet* packet = packet_alloc(fromDevice->entityId, fromDevice->address, targetDevice->address, exploit->flavorText);
    packet->exploit = exploit;
    entity_send_packet_from_entity(targetDevice->entityId, packet);
}

void worm_system_update_infection(char* entityId, Infection* infection) {
    KnownHosts* knownHosts = g_hash_table_lookup(componentRegistry.knownHosts, entityId);
    Device* device = g_hash_table_lookup(componentRegistry.devices, entityId);
    if (!knownHosts || !device) return;

    for (int i = 0; i < infection->numWorms; i++) {
        Worm* worm = infection->worms[i];
        int activeSlot = infection->activeSlots[i];

        switch (worm->slots[activeSlot].type) {
            case WormSlotRemoteExploit: {
                Exploit* exploit = worm->slots[activeSlot].content.exploit;
                for (int j = 0; j < knownHosts->numEntities; j++) {
                    char* targetEntityId = knownHosts->entities[j];
                    Device* targetDevice = g_hash_table_lookup(componentRegistry.devices, targetEntityId);
                    if (!targetDevice) continue;

                    worm_system_try_remote_exploit(device, targetDevice, exploit);
                }
                // TODO: Have packets drive remote exploit, just cuz it's cool
                break;
            }
            default:
                break;
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

    while (wormState->worm->slots[wormState->curActiveSlot].type == WormSlotEmpty) {
        wormState->worm->numSlots++;
        if (wormState->curActiveSlot > wormState->worm->numSlots) {
            log_debug("Worm %s finished loop, cooling down", wormState->worm->wormName);
            wormState->curActiveSlot = 0;
            wormState->state = WORM_STATE_COOLDOWN;
        }
    }

    switch (currentWormSlot.type) {
        case WormSlotRemoteExploit: {
            int targetIdx = wormState->targetsCompleted-1;
            if (wormState->targetsCompleted >= wormState->numTargetDevices) {
                isCurrentSlotDone = true;
                break;
            }
//            worm_system_try_remote_exploit(currentWormSlot.content.exploit, wormState->targetDevices[targetIdx]);
            wormState->targetsCompleted++;
            break;
        }
        case WormSlotCredentialAttack:
            break;
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

void worm_system_init_worm_state(Worm* worm) {
    int i = wormSystemState.numWorms;
    wormSystemState.wormStates[i].worm = worm;
    wormSystemState.wormStates[i].state = WORM_STATE_INACTIVE;
    wormSystemState.wormStates[i].curActiveSlot = 0;
    wormSystemState.wormStates[i].numTargetDevices = 0;
    wormSystemState.wormStates[i].numInfectedDevices = 0;
    wormSystemState.wormStates[i].targetsInfected = 0;
    wormSystemState.wormStates[i].targetsCompleted = 0;
    StartTimer(&wormSystemState.wormStates[i].timer, WORM_SYSTEM_TIMER_INTERVAL);
    wormSystemState.numWorms = i+1;
}

void worm_system_add_worm(WormEvent event) {
    int i = worldState.numWorms;
    worldState.worms[worldState.numWorms++] = event.worm;
    worm_system_init_worm_state(&worldState.worms[i]);
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
