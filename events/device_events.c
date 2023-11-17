#include <glib.h>
#include "device_events.h"

GList _deviceEventListeners;
GList* deviceEventListeners = &_deviceEventListeners;

void events_register_device_event_listener(void (*eventHandler)(DeviceEvent)) {
    if (!g_list_find(deviceEventListeners, eventHandler)) {
        g_list_append(deviceEventListeners, eventHandler);
    }
}

void events_unregister_device_event_listener(void (*eventHandler)(DeviceEvent)) {
    g_list_remove(deviceEventListeners, eventHandler);
}

void events_publish_device_event(char* entityId, Device* device, enum EventType type) {
    DeviceEvent event = (DeviceEvent){type, entityId, device};

    GList* list = deviceEventListeners->next;
    while (list && list->data) {
        void (*eventHandler)(DeviceEvent) = list->data;
        eventHandler(event);
        list = list->next;
    }

    events_add_event_log_message_char(type, entityId, device->address);
}
