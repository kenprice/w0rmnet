#include <stdlib.h>
#include <string.h>
#include "glib.h"
#include "component_registry.h"
#include "process.h"

void register_process_manager(ProcessManager process_manager, char* entity_id) {
    ProcessManager* new_process_manager = calloc(1, sizeof(ProcessManager));
    memcpy(new_process_manager, &(process_manager), sizeof(ProcessManager));
    g_hash_table_insert(component_registry.process_managers, entity_id, new_process_manager);
}
