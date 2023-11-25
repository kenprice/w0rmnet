#include <math.h>
#include "glib.h"
#include "worm_system.h"
#include "../components/component_registry.h"
#include "../components/device.h"
#include "../components/infection.h"
#include "../components/known_hosts.h"
#include "../entities/helpers.h"
#include "../events/worm_events.h"
#include "../lib/log/log.h"
#include "../store/cred_dumps.h"
#include "../store/exploits.h"
#include "../utils/timer.h"
#include "../world/world_state.h"

WormSystemState wormSystemState;

#define WORM_SYSTEM_TIMER_COOLDOWN 5.0f
#define WORM_SYSTEM_TIMER_INTERVAL 2.0f

void worm_system_update_infection(char* entityId, Infection* infection);
void worm_system_handle_add_worm_event(WormEvent event);
void worm_system_handle_worm_infects_device_event(WormEvent event);
static void worm_system_try_credential_attack(Device* fromDevice, Device* targetDevice, Infection* infection, Worm* worm, CredDump* credDump);
static void worm_system_try_remote_exploit(Device* fromDevice, Device* targetDevice, Worm* worm, Exploit* exploit);
static WormState* worm_system_get_worm_state_for_worm(Worm* worm);

void init_worm_system() {
    log_debug("Initializing w0rm system");
    for (int i = 0; i < worldState.numWorms; i++) {
        log_debug("Initialized worm %s", worldState.worms[i].wormName);
        wormSystemState.wormStates[i].worm = &worldState.worms[i];
        wormSystemState.wormStates[i].numInfectedDevices = 0;
    }
    wormSystemState.numWorms = worldState.numWorms;

    events_register_worm_event_listener(worm_system_handle_add_worm_event);
    events_register_worm_event_listener(worm_system_handle_worm_infects_device_event);
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

void worm_system_update_infection(char* entityId, Infection* infection) {
    KnownHosts* knownHosts = g_hash_table_lookup(componentRegistry.knownHosts, entityId);
    Device* device = g_hash_table_lookup(componentRegistry.devices, entityId);
    if (!knownHosts || !device || knownHosts->numEntities == 0) return;

    for (int i = 0; i < infection->numWorms; i++) {
        bool nextActiveSlot = false;
        Worm* worm = infection->worms[i];
        int activeSlot = infection->activeSlots[i];

        if (!TimerDone(infection->runnerTimer)) return;

        switch (worm->slots[activeSlot].type) {
            case WormSlotRemoteExploit: {
                Exploit* exploit = worm->slots[activeSlot].content.exploit;
                for (int j = 0; j < knownHosts->numEntities; j++) {
                    char* targetEntityId = knownHosts->entities[j];
                    Device* targetDevice = g_hash_table_lookup(componentRegistry.devices, targetEntityId);
                    if (!targetDevice) continue;

                    worm_system_try_remote_exploit(device, targetDevice, worm, exploit);
                }
                StartTimer(&infection->runnerTimer, (1.0f + log(knownHosts->numEntities)) * 1.5f);
                nextActiveSlot = true;
                break;
            }
            case WormSlotCredentialAttack: {
                // TODO: Smarter credential attack; ignore target if no login; maybe worm lifecycle (start, do, end)
                CredDump* credDump = worm->slots[activeSlot].content.credDump;
                for (int j = 0; j < knownHosts->numEntities; j++) {
                    char* targetEntityId = knownHosts->entities[j];
                    Device* targetDevice = g_hash_table_lookup(componentRegistry.devices, targetEntityId);
                    if (!targetDevice) continue;

                    worm_system_try_credential_attack(device, targetDevice, infection, worm, credDump);
                }
                infection->curCred++;
                if (infection->curCred >= credDump->numCredentials) {
                    StartTimer(&infection->runnerTimer, 5.0f);
                    infection->curCred = 0;
                    nextActiveSlot = true;
                }
                StartTimer(&infection->runnerTimer, (1.0f + log(knownHosts->numEntities)) * 1.5f);
                break;
            }
            default:
                nextActiveSlot = true;
                break;
        }

        if (nextActiveSlot) {
            infection->activeSlots[i]++;
            if (infection->activeSlots[i] >= worm->numSlots) infection->activeSlots[i] = 0;
        }
    }
}

void worm_system_init_worm_state(Worm* worm) {
    int i = wormSystemState.numWorms;
    wormSystemState.wormStates[i].worm = worm;
    wormSystemState.wormStates[i].numInfectedDevices = 0;
    wormSystemState.numWorms = i+1;
}

void worm_system_handle_add_worm_event(WormEvent event) {
    if (event.type != WormCreatedEvent) return;

    int i = worldState.numWorms;
    worldState.worms[worldState.numWorms++] = *event.worm;
    worm_system_init_worm_state(&worldState.worms[i]);
}

void worm_system_handle_worm_infects_device_event(WormEvent event) {
    if (event.type != WormInfectsDeviceEvent) return;

    WormState* wormState = worm_system_get_worm_state_for_worm(event.worm);
    Device* targetDevice = event.device;
    Infection* infection = g_hash_table_lookup(componentRegistry.infections, targetDevice->entityId);

    if (!wormState) {
        log_debug("Could not find worm state for worm infection event. %s", event.worm->wormName);
        return;
    }

    for (int i = 0; i <  wormState->numInfectedDevices; i++) {
        if (wormState->infectedDevices[i] == targetDevice) {
            log_debug("Device %s is already infected with %s", targetDevice->address, wormState->worm->wormName);
            return;
        }
    }

    if (!infection) {
        infection = calloc(1, sizeof(Infection));
        g_hash_table_insert(componentRegistry.infections, targetDevice->entityId, infection);
        infection->numWorms = 0;
    }

    if (!comp_infection_add_worm(targetDevice->entityId, wormState->worm)) {
        log_debug("Device %s is already infected with %s", targetDevice->address, wormState->worm->wormName);
        return;
    }

    log_debug("Infected %s with %s", targetDevice->address, wormState->worm->wormName);
    wormState->infectedDevices[wormState->numInfectedDevices++] = targetDevice;
}

static void worm_system_try_remote_exploit(Device* fromDevice, Device* targetDevice, Worm* worm, Exploit* exploit) {
    WormState* wormState = worm_system_get_worm_state_for_worm(worm);
    for (int i = 0; i <  wormState->numInfectedDevices; i++) {
        if (wormState->infectedDevices[i] == targetDevice) return;
    }
    log_debug("Infection on %s, Worm %s, attempts exploit on %s", fromDevice->address, worm->wormName, targetDevice->address);

    Packet* packet = packet_alloc(fromDevice->entityId, fromDevice->address, targetDevice->address, exploit->flavorText);
    packet->payload.worm = worm;
    packet->payload.exploit = exploit;
    entity_send_packet_from_entity(targetDevice->entityId, packet);
}

static void worm_system_try_credential_attack(Device* fromDevice, Device* targetDevice, Infection* infection, Worm* worm, CredDump* credDump) {
    WormState* wormState = worm_system_get_worm_state_for_worm(worm);
    for (int i = 0; i <  wormState->numInfectedDevices; i++) {
        if (wormState->infectedDevices[i] == targetDevice) return;
    }

    char** credentialsList = g_strsplit(credDump->contents, "\n", credDump->numCredentials);
    char* curCred = credentialsList[infection->curCred];
    char buffer[200];
    sprintf(buffer, "LOGIN %s", curCred);
    Packet* packet = packet_alloc(fromDevice->entityId, fromDevice->address, targetDevice->address, buffer);
    packet->payload.worm = worm;
    entity_send_packet_from_entity(targetDevice->entityId, packet);
    // TODO: event for infection via cred attack by worm
    log_debug("Infection on %s, Worm %s, attempts LOGIN %s on %s", fromDevice->address, worm->wormName, curCred, targetDevice->address);

    g_strfreev(credentialsList);
}

static WormState* worm_system_get_worm_state_for_worm(Worm* worm) {
    for (int i = 0; i < wormSystemState.numWorms; i++) {
        if (wormSystemState.wormStates[i].worm == worm) {
            return &wormSystemState.wormStates[i];
        }
    }
}
