#include "world_state_system.h"
#include "../components/component_registry.h"
#include "../components/loot.h"
#include "../events/events.h"
#include "../events/device_events.h"
#include "../world/world_state.h"

void world_state_system_on_device_pwned(DeviceEvent event);

void init_world_state_system() {
    init_world_state();
    events_register_device_event_listener(world_state_system_on_device_pwned);
}

void update_world_state_system() {

}

void world_state_system_on_device_pwned(DeviceEvent event) {
    if (event.type == DevicePwnedEvent) {
        Loot* loot = g_hash_table_lookup(componentRegistry.loots, event.deviceId);
        if (!loot) return;

        if (loot->bitCredits) {
            worldState.bitCredits += loot->bitCredits;
        }

        g_hash_table_remove(componentRegistry.loots, event.deviceId);
    }
}
