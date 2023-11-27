#ifndef W0RMNET_PLAYER_EVENTS_H
#define W0RMNET_PLAYER_EVENTS_H

#include "events.h"
#include "../store/exploits.h"
#include "../store/cred_dumps.h"

typedef union {
    int bitCredits;
    Exploit* exploit;
    CredDump* credDump;
} PlayerEventMessage;

typedef struct {
    EventType type;
    PlayerEventMessage message;
} PlayerEvent;

void events_register_player_event_listener(void (*eventHandler)(PlayerEvent));

void events_unregister_player_event_listener(void (*eventHandler)(PlayerEvent));

void events_publish_player_bitcredit_event(int bitCredits);

void events_publish_player_exploit_event(Exploit* exploit);

void events_publish_player_cred_dump_event(CredDump* credDump);

#endif //W0RMNET_PLAYER_EVENTS_H
