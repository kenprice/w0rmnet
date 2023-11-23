#include "world_state_system.h"
#include "../components/component_registry.h"
#include "../components/loot.h"
#include "../events/events.h"
#include "../events/device_events.h"
#include "../events/player_events.h"
#include "../world/world_state.h"

void world_state_system_on_device_event(DeviceEvent event);
void world_state_system_on_player_event(PlayerEvent event);

void init_world_state_system() {
    init_world_state();
    events_register_device_event_listener(world_state_system_on_device_event);
    events_register_player_event_listener(world_state_system_on_player_event);
}

void update_world_state_system() {

}

void world_state_system_on_device_event(DeviceEvent event) {
    if (event.type == DevicePwnedViaExploit || event.type == DevicePwnedViaLogin) {
        events_add_event_log_message_char_char(event.type, event.deviceId, event.device->address, event.fromDevice->address);
    } else {
        events_add_event_log_message_char(event.type, event.deviceId, event.device->address);
    }

    if (event.type == DevicePwnedEvent || event.type == DevicePwnedViaExploit || event.type == DevicePwnedViaLogin) {
        Loot* loot = g_hash_table_lookup(componentRegistry.loots, event.deviceId);
        if (!loot) return;

        if (loot->bitCredits) {
            worldState.bitCredits += loot->bitCredits;
            events_publish_player_bitcredit_event(loot->bitCredits);
        }


        g_hash_table_remove(componentRegistry.loots, event.deviceId);
    }
}

void world_state_system_on_player_event(PlayerEvent event) {
    switch (event.type) {
        case PlayerReceivesBitCreditsEvent:
            events_add_event_log_message_int(PlayerReceivesBitCreditsEvent, NULL, event.message.bitCredits);
            break;

        case PlayerReceivesExploitEvent:
        case PlayerReceivesCredDumpEvent:
        default:
            break;
    }
}
