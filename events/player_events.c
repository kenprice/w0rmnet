#include <glib.h>
#include "player_events.h"

GList _playerEventListeners;
GList* playerEventListeners = &_playerEventListeners;

void events_publish_player_event(PlayerEvent event);

void events_register_player_event_listener(void (*eventHandler)(PlayerEvent)) {
    if (!g_list_find(playerEventListeners, eventHandler)) {
        g_list_append(playerEventListeners, eventHandler);
    }
}

void events_unregister_player_event_listener(void (*eventHandler)(PlayerEvent)) {
    g_list_remove(playerEventListeners, eventHandler);
}

void events_publish_player_bitcredit_event(int bitCredits) {
    PlayerEventMessage message;
    message.bitCredits = bitCredits;
    PlayerEvent event = (PlayerEvent){ PlayerReceivesBitCreditsEvent, message };
    events_publish_player_event(event);
}

void events_publish_player_exploit_event(Exploit* exploit) {
    PlayerEventMessage message;
    message.exploit = exploit;
    PlayerEvent event = (PlayerEvent){ PlayerReceivesExploitEvent, message };
    events_publish_player_event(event);
}

void events_publish_player_cred_dump_event(CredDump* credDump) {
    PlayerEventMessage message;
    message.credDump = credDump;
    PlayerEvent event = (PlayerEvent){ PlayerReceivesCredDumpEvent, message };
    events_publish_player_event(event);
}

void events_publish_player_event(PlayerEvent event) {
    GList* list = playerEventListeners->next;
    while (list && list->data) {
        void (*eventHandler)(PlayerEvent) = list->data;
        eventHandler(event);
        list = list->next;
    }
}
