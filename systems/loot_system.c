#include "loot_system.h"
#include "../components/component_registry.h"
#include "../components/loot.h"
#include "../events/events.h"
#include "../world/world_state.h"

void loot_system_on_device_pwned(DeviceEvent event);

void init_loot_system() {
    events_register_device_event_listener(loot_system_on_device_pwned);
}

void loot_system_on_device_pwned(DeviceEvent event) {
    if (event.type != DevicePwnedEvent) return;

    Loot* loot = g_hash_table_lookup(componentRegistry.loots, event.deviceId);
    if (!loot) return;

    if (loot->bitCredits) {
        worldState.bitCredits += loot->bitCredits;
    }

    g_hash_table_remove(componentRegistry.loots, event.deviceId);
}
