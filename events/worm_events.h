#ifndef W0RMNET_WORM_EVENTS_H
#define W0RMNET_WORM_EVENTS_H

#include "events.h"
#include "../store/worms.h"

typedef struct {
    EventType type;
    Worm worm;
} WormEvent;

void events_register_worm_event_listener(void (*eventHandler)(WormEvent));

void events_unregister_worm_event_listener(void (*eventHandler)(WormEvent));

void events_publish_worm_created_event(Worm worm);

#endif //W0RMNET_WORM_EVENTS_H
