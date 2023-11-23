#ifndef W0RMNET_PLAYER_EVENTS_H
#define W0RMNET_PLAYER_EVENTS_H

#include "events.h"

typedef union {
    int bitCredits;
    const char* exploit;
    const char* credDump;
} PlayerEventMessage;

typedef struct {
    EventType type;
    PlayerEventMessage message;
} PlayerEvent;

void events_register_player_event_listener(void (*eventHandler)(PlayerEvent));

void events_unregister_player_event_listener(void (*eventHandler)(PlayerEvent));

void events_publish_player_bitcredit_event(int bitCredits);

void events_publish_player_exploit_event(const char* exploit);

void events_publish_player_cred_dump_event(const char* credDump);

#endif //W0RMNET_PLAYER_EVENTS_H
