#ifndef W0RMNET_DEVICE_EVENTS_H
#define W0RMNET_DEVICE_EVENTS_H

#include "events.h"
#include "../components/device.h"

typedef struct {
    EventType type;
    char* deviceId;
    Device* device;
    Device* fromDevice; // The device that caused the event, e.g. [fromDevice] pwned [device]
} DeviceEvent;

void events_register_device_event_listener(void (*eventHandler)(DeviceEvent));

void events_unregister_device_event_listener(void (*eventHandler)(DeviceEvent));

void events_publish_device_event(char* entityId, Device* device, EventType type);

void events_publish_device_pwned_device_via_exploit_event(Device* fromDevice, char* toEntityId, Device* toDevice);

void events_publish_device_pwned_device_via_login_event(Device* fromDevice, char* toEntityId, Device* toDevice);

#endif //W0RMNET_DEVICE_EVENTS_H
