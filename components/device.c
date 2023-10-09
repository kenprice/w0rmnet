#include <glib.h>
#include <string.h>
#include "device.h"

char* find_device_entity_id_by_device_id(ComponentRegistry* registry, char* device_id) {
    GHashTableIter iter;
    g_hash_table_iter_init(&iter, registry->devices);

    guint* key_;
    Device* device;

    while (g_hash_table_iter_next (&iter, (gpointer) &key_, (gpointer) &device)) {
        if (strcmp(device->id, device_id) == 0) {
            return device->entity_id;
        }
    }

    return NULL;
}

void register_device(ComponentRegistry* registry, Device device, char* entity_id) {
    Device* new_device = calloc(1, sizeof(Device));
    memcpy(new_device, &(device), sizeof(Device));
    strncpy(new_device->entity_id, entity_id, UUID_STR_LEN);
    g_hash_table_insert(registry->devices, entity_id, new_device);
}
