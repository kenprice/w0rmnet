#include <glib.h>
#include "worm_events.h"

GList _wormEventListeners;
GList* wormEventListeners = &_wormEventListeners;

void events_register_worm_event_listener(void (*eventHandler)(WormEvent)) {
    if (!g_list_find(wormEventListeners, eventHandler)) {
        g_list_append(wormEventListeners, eventHandler);
    }
}

void events_unregister_worm_event_listener(void (*eventHandler)(WormEvent)) {
    g_list_remove(wormEventListeners, eventHandler);
}

void events_worm_call_event_handlers(WormEvent event) {
    GList* list = wormEventListeners->next;
    while (list && list->data) {
        void (*eventHandler)(WormEvent) = list->data;
        eventHandler(event);
        list = list->next;
    }
}

void events_publish_worm_created_event(Worm* worm) {
    WormEvent event = { WormCreatedEvent, worm };
    events_worm_call_event_handlers(event);
}

void events_publish_worm_infected_device_event(Worm* worm, Device* device) {
    WormEvent event = { WormInfectsDeviceEvent, worm, device };
    events_worm_call_event_handlers(event);
}

