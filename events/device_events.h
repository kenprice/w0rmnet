#ifndef W0RMNET_DEVICE_EVENTS_H
#define W0RMNET_DEVICE_EVENTS_H

#include "events.h"
#include "../components/device.h"

typedef struct {
    enum EventType type;
    char* deviceId;
    Device* device;
} DeviceEvent;

void events_register_device_event_listener(void (*eventHandler)(DeviceEvent));

void events_unregister_device_event_listener(void (*eventHandler)(DeviceEvent));

void events_publish_device_event(char* entityId, Device* device, enum EventType type);

#endif //W0RMNET_DEVICE_EVENTS_H
