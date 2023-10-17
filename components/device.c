#include <glib.h>
#include <string.h>
#include "device.h"
#include "component_registry.h"
#include "components.h"

const char* DeviceTypeLabel[2] = {
        "Machine",
        "Router"
};

char* find_device_entity_id_by_device_id(char* deviceId) {
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, componentRegistry.devices);

    guint* key_;
    Device* device;

    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &device)) {
        if (strcmp(device->name, deviceId) == 0) {
            return device->entityId;
        }
    }

    return NULL;
}

Device* find_device_by_coord(int x, int y) {
    GHashTableIter iter;
    char* entityId;
    Device* device;

    g_hash_table_iter_init(&iter, componentRegistry.devices);
    while (g_hash_table_iter_next (&iter, (gpointer) &entityId, (gpointer) &device)) {
        Position* position = (Position*)g_hash_table_lookup(componentRegistry.positions, entityId);
        if (position->coord.x == x && position->coord.y == y) {
            return device;
        }
    }

    return NULL;
}

void register_device(Device device, char* entityId) {
    Device* newDevice = calloc(1, sizeof(Device));
    memcpy(newDevice, &(device), sizeof(Device));
    strncpy(newDevice->entityId, entityId, UUID_STR_LEN);
    g_hash_table_insert(componentRegistry.devices, entityId, newDevice);
}

void iterate_devices(void (*cb)(char*,Device*)) {
    GHashTableIter iter;
    char* entity_id;
    Device* device;

    g_hash_table_iter_init(&iter, componentRegistry.devices);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &device)) {
        (*cb)(entity_id, device);
    }
}
