#ifndef W0RMNET_WORM_EVENTS_H
#define W0RMNET_WORM_EVENTS_H

#include "events.h"
#include "../components/device.h"
#include "../store/worms.h"

typedef struct {
    EventType type;
    Worm* worm;
    Device* device;
} WormEvent;

void events_register_worm_event_listener(void (*eventHandler)(WormEvent));

void events_unregister_worm_event_listener(void (*eventHandler)(WormEvent));

void events_publish_worm_created_event(Worm* worm);

void events_publish_worm_infected_device_event(Worm* worm, Device* device);

#endif //W0RMNET_WORM_EVENTS_H
