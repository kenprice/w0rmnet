#include <glib.h>
#include "device_events.h"

GList _deviceEventListeners;
GList* deviceEventListeners = &_deviceEventListeners;

void events_call_event_handlers(DeviceEvent event);

void events_register_device_event_listener(void (*eventHandler)(DeviceEvent)) {
    if (!g_list_find(deviceEventListeners, eventHandler)) {
        g_list_append(deviceEventListeners, eventHandler);
    }
}

void events_unregister_device_event_listener(void (*eventHandler)(DeviceEvent)) {
    g_list_remove(deviceEventListeners, eventHandler);
}

void events_publish_device_event(char* entityId, Device* device, EventType type) {
    DeviceEvent event = { type, entityId, device };
    events_call_event_handlers(event);
}

void events_publish_device_pwned_device_via_exploit_event(Device* fromDevice, char* toEntityId, Device* toDevice) {
    DeviceEvent event = { DevicePwnedViaExploit, toEntityId, toDevice, fromDevice };
    events_call_event_handlers(event);
}

void events_publish_device_pwned_device_via_login_event(Device* fromDevice, char* toEntityId, Device* toDevice) {
    DeviceEvent event = { DevicePwnedViaLogin, toEntityId, toDevice, fromDevice };
    events_call_event_handlers(event);
}

void events_call_event_handlers(DeviceEvent event) {
    GList* list = deviceEventListeners->next;
    while (list && list->data) {
        void (*eventHandler)(DeviceEvent) = list->data;
        eventHandler(event);
        list = list->next;
    }
}

