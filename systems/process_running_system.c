#include "process_running_system.h"
#include "../components/components.h"
#include "../components/component_registry.h"

void proc_ping_handle_packet(char* entity_id, Packet* packet) {
    if (packet == NULL) return;

    bool isValid = false;

    if (strcmp(packet->message, "Ping?") == 0) {
        PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, entity_id);
        packet_queue_write(&packet_buffer->send_q, packet_alloc(entity_id, packet->from_address, "Pong!"));
        isValid = true;
    }

    if (strcmp(packet->message, "Pong!") == 0) {
        isValid = true;
    }

    // Received packet is valid ping-pong packet, make sender visible if not already
    if (isValid) {
        char* from_entity_id = packet->from_entity_id;
        Device* device = (Device*)g_hash_table_lookup(component_registry.devices, from_entity_id);
        if (device && !device->visible) {
            device->visible = true;
        }
    }
}

void proc_ping_handle_message(char* entity_id, ProcMessage* message) {
    if (message == NULL) return;
    if (strlen(message->args) == 0) return;

    // Sends Ping to address specified by args
    char* address = message->args;
    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, entity_id);
    packet_queue_write(&packet_buffer->send_q, packet_alloc(entity_id, address, "Ping?"));
}

void send_packet_to_proc(char* entity_id, Process* process, Packet* packet) {
    switch (process->type) {
        case PROCESS_TYPE_PING:
            proc_ping_handle_packet(entity_id, packet);
            break;
    }
}

void send_message_to_proc(char* entity_id, Process* process, ProcMessage* message) {
    switch (process->type) {
        case PROCESS_TYPE_PING:
            proc_ping_handle_message(entity_id, message);
            break;
    }
}

void update_process_manager(char* entity_id, ProcessManager* process_manager) {
    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(component_registry.packet_buffers, entity_id);
    Packet* packet = NULL;

    // A packet, if exists, is broadcasted to all processes. Something that no sane systems engineer would do.
    if (packet_buffer != NULL) {
        packet = packet_queue_read(&packet_buffer->recv_q);
    }
    for (int i = 0; i < process_manager->num_procs; i++) {
        send_packet_to_proc(entity_id, &process_manager->processes[i], packet);
    }

    // Then, go through IPC proc msg Q and send messages to procs
    ProcMessageQueue* procMessageQueue = (ProcMessageQueue*)g_hash_table_lookup(component_registry.proc_msg_queues, entity_id);
    ProcMessage* message = NULL;
    if (procMessageQueue != NULL) {
        message = proc_msg_queue_read(procMessageQueue);
        if (message != NULL) {
            Process proc = process_manager->processes[message->pid];
            if (proc.invokable) {
                send_message_to_proc(entity_id, &proc, message);
            }
        }
    }
}

void update_process_running_system() {
    GHashTableIter iter;
    char* entity_id;
    ProcessManager* process_manager;

    g_hash_table_iter_init(&iter, component_registry.process_managers);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &process_manager)) {
        update_process_manager(entity_id, process_manager);
    }
}
