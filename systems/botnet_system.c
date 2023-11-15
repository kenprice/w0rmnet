#include <stdio.h>
#include "botnet_system.h"
#include "../events/events.h"
#include "../components/component_registry.h"
#include "../world/world_map.h"

BotnetState botnetState;

void botnet_system_on_device_pwned(DeviceEvent event);

void update_botnet_system() {
    if (!botnetState.state || !TimerDone(botnetState.timer)) return;

    for (int i = 0; i < botnetState.numOwnedDevices; i++) {
        Device* device = botnetState.ownedDevices[i];
        ProcessManager* processManager = g_hash_table_lookup(componentRegistry.processManagers, device->entityId);
        ProcMessageQueue* procMessageQueue = g_hash_table_lookup(componentRegistry.procMsgQueues, device->entityId);

        if (!processManager || !procMessageQueue) continue;

        int procIdx = -1;
        for (int j = 0; j < processManager->numProcs; j++) {
            if (processManager->processes[j].program.type == ProgramTypeLogin) {
                procIdx = j;
                break;
            }
        }
        if (procIdx == -1) continue;

        char buffer[100];
        sprintf(buffer, "%s:root:root", botnetState.targetDevices[botnetState.targetsCompleted++]->address);
        ProcMessage* msg = proc_msg_alloc(procIdx, buffer);
        proc_msg_queue_write(procMessageQueue, msg);

        if (botnetState.targetsCompleted >= botnetState.numTargetDevices) {
            // Finished
            botnetState.state = BOTNET_STATE_STANDBY;
            events_unregister_device_event_listener(botnet_system_on_device_pwned);
            return;
        }
    }

    StartTimer(&botnetState.timer, 1.5);
}

/**
 * Takes N owned machines, and sends LOGIN root:root to all visible machines
 */
void botnet_system_test_launch_login_attack() {
    GHashTableIter iter;
    char* entityId;
    Device* device;

    botnetState.numOwnedDevices = 0;
    botnetState.numTargetDevices = 0;
    botnetState.targetsCompleted = 0;
    botnetState.targetsPwned = 0;

    g_hash_table_iter_init(&iter, componentRegistry.devices);
    while (g_hash_table_iter_next(&iter, (gpointer) &entityId, (gpointer) &device)) {
        if (device->type == DEVICE_TYPE_ROUTER) continue;
        if (device->owner == DEVICE_OWNER_PLAYER) {
            botnetState.ownedDevices[botnetState.numOwnedDevices++] = device;
        } else if (device->visible) {
            botnetState.targetDevices[botnetState.numTargetDevices++] = device;
        }
    }

    botnetState.state = BOTNET_STATE_ACTIVE;
    StartTimer(&botnetState.timer, 1.5);

    events_register_device_event_listener(botnet_system_on_device_pwned);
}

void botnet_system_on_device_pwned(DeviceEvent event) {
    if (event.type != DevicePwnedEvent) return;
    botnetState.targetsPwned++;
}
