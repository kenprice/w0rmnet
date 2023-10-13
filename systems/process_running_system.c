#include "process_running_system.h"
#include "../components/components.h"
#include "../components/component_registry.h"

void run_ping(char* entity_id) {
    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, entity_id);
    if (packet_buffer == NULL) return;

    Packet* packet = packet_queue_peek(&packet_buffer->recv_q);
    if (packet == NULL) return;

    if (strcmp(packet->message, "Ping?") == 0) {
        packet = packet_queue_read(&packet_buffer->recv_q);
        packet_queue_write(&packet_buffer->send_q, packet_alloc(packet->from_address, "Pong!"));
    }
}

void update_process(char* entity_id, Process* process) {
    switch (process->type) {
        case PROCESS_TYPE_PING:
            run_ping(entity_id);
            break;
    }
}

void update_process_running_system() {
    GHashTableIter iter;
    char* entity_id;
    ProcessManager* process_manager;

    g_hash_table_iter_init(&iter, component_registry.process_managers);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &process_manager)) {
        for (int i = 0; i < process_manager->num_procs; i++) {
            update_process(entity_id, &process_manager->processes[i]);
        }
    }
}
