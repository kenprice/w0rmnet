#include <glib.h>
#include <string.h>
#include "device.h"
#include "component_registry.h"
#include "components.h"

const char* DeviceTypeLabel[2] = {
        "Machine",
        "Router"
};

char* find_device_entity_id_by_device_id(char* device_id) {
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, component_registry.devices);

    guint* key_;
    Device* device;

    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &device)) {
        if (strcmp(device->id, device_id) == 0) {
            return device->entity_id;
        }
    }

    return NULL;
}

Device* find_device_by_coord(int x, int y) {
    GHashTableIter iter;
    char* entity_id;
    Device* device;

    g_hash_table_iter_init(&iter, component_registry.devices);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &device)) {
        Position* position = (Position*)g_hash_table_lookup(component_registry.positions, entity_id);
        if (position->coord.x == x && position->coord.y == y) {
            return device;
        }
    }

    return NULL;
}

void register_device(Device device, char* entity_id) {
    Device* new_device = calloc(1, sizeof(Device));
    memcpy(new_device, &(device), sizeof(Device));
    strncpy(new_device->entity_id, entity_id, UUID_STR_LEN);
    g_hash_table_insert(component_registry.devices, entity_id, new_device);
}

void iterate_devices(void (*cb)(char*,Device*)) {
    GHashTableIter iter;
    char* entity_id;
    Device* device;

    g_hash_table_iter_init(&iter, component_registry.devices);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &device)) {
        (*cb)(entity_id, device);
    }
}
