#include "process_running_system.h"
#include "../components/components.h"
#include "../components/component_registry.h"

#include "process_runner/ping_runner.h"
#include "process_runner/netscan_runner.h"
#include "process_runner/login_runner.h"

void send_packet_to_proc(char* entity_id, Process* process, Packet* packet) {
    switch (process->type) {
        case PROCESS_TYPE_PING:
            proc_ping_handle_packet(entity_id, process, packet);
            break;
        case PROCESS_TYPE_LOGIN:
            proc_login_handle_packet(entity_id, process, packet);;
        default:
            break;
    }
}

void send_message_to_proc(char* entity_id, Process* process, ProcMessage* message) {
    switch (process->type) {
        case PROCESS_TYPE_PING:
            proc_ping_handle_message(entity_id, process, message);
            break;
        case PROCESS_TYPE_SCAN:
            proc_netscan_handle_message(entity_id, process, message);
            break;
        case PROCESS_TYPE_LOGIN:
            proc_login_handle_message(entity_id, process, message);
            break;
        default:
            break;
    }
}

void update_process_manager(char* entity_id, ProcessManager* process_manager) {
    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entity_id);
    Packet* packet = NULL;

    // A packet, if exists, is broadcasted to all processes. Something that no sane systems engineer would do.
    if (packet_buffer != NULL) {
        packet = packet_queue_read(&packet_buffer->recvQ);
    }
    for (int i = 0; i < process_manager->numProcs; i++) {
        send_packet_to_proc(entity_id, &process_manager->processes[i], packet);
    }

    // Then, go through IPC proc msg Q and send messages to procs
    ProcMessageQueue* procMessageQueue = (ProcMessageQueue*)g_hash_table_lookup(componentRegistry.procMsgQueues, entity_id);
    ProcMessage* message = NULL;
    if (procMessageQueue != NULL) {
        message = proc_msg_queue_read(procMessageQueue);
        if (message != NULL) {
            Process proc = process_manager->processes[message->pid];
            if (proc.invocable) {
                send_message_to_proc(entity_id, &proc, message);
            }
        }
    }
}

void update_process_running_system() {
    GHashTableIter iter;
    char* entity_id;
    ProcessManager* process_manager;

    g_hash_table_iter_init(&iter, componentRegistry.processManagers);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &process_manager)) {
        update_process_manager(entity_id, process_manager);
    }
}
