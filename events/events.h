#ifndef W0RMNET_EVENTS_H
#define W0RMNET_EVENTS_H

#include "../components/device.h"

enum DeviceEventType {
    DeviceDiscoveredEvent,
    DevicePwnedEvent,
};

typedef struct {
    enum DeviceEventType type;
    char* deviceId;
    Device* device;
} DeviceEvent;

void events_register_device_event_listener(void (*eventHandler)(DeviceEvent));

void events_unregister_device_event_listener(void (*eventHandler)(DeviceEvent));

void events_publish_device_event(char* entityId, Device* device, enum DeviceEventType type);

#endif //W0RMNET_EVENTS_H
