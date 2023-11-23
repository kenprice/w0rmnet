#include "process_running_system.h"
#include "../components/components.h"
#include "../components/component_registry.h"
#include "../events/device_events.h"
#include "process_runner/ping_runner.h"
#include "process_runner/netscan_runner.h"
#include "process_runner/login_runner.h"
#include "process_runner/check_origin_runner.h"

static void send_packet_to_proc(char* entityId, Process* process, Packet* packet);
static void send_message_to_proc(char* entityId, Process* process, ProcMessage* message);
static void run_service_procs(char* entityId, Process* process);
static void update_process_manager(char* entityId, ProcessManager* processManager);

void update_process_running_system() {
    GHashTableIter iter;
    char* entity_id;
    ProcessManager* process_manager;

    g_hash_table_iter_init(&iter, componentRegistry.processManagers);
    while (g_hash_table_iter_next (&iter, (gpointer) &entity_id, (gpointer) &process_manager)) {
        update_process_manager(entity_id, process_manager);
    }
}

static void check_packet_for_exploit(char* entityId, Process* proc, Packet* packet) {
    Device* targetDevice = g_hash_table_lookup(componentRegistry.devices, entityId);
    if (!proc || !packet || !targetDevice || targetDevice->owner == DEVICE_OWNER_PLAYER) return;

    Exploit* exploit = packet->exploit;
    if (!exploit) return;

    bool nameMatch = strcmp(proc->program.name, exploit->programName) == 0;
    bool versionMatch = proc->program.version == exploit->programVersion;

    if (nameMatch && versionMatch) {
        Device* fromDevice = g_hash_table_lookup(componentRegistry.devices, packet->fromEntityId);
        targetDevice->owner = DEVICE_OWNER_PLAYER;
        events_publish_device_pwned_device_via_exploit_event(fromDevice, entityId, targetDevice);
    }
}

static void update_process_manager(char* entityId, ProcessManager* processManager) {
    PacketBuffer* packet_buffer = (PacketBuffer*)g_hash_table_lookup(componentRegistry.packetBuffers, entityId);
    Packet* packet = NULL;

    if (packet_buffer != NULL) {
        packet = packet_queue_read(&packet_buffer->recvQ);
    }
    for (int i = 0; i < processManager->numProcs; i++) {
        Process* proc = &processManager->processes[i];
        // A packet, if exists, is broadcasted to all processes. Something that no sane systems engineer would do.
        if (packet) {
            check_packet_for_exploit(entityId, proc, packet);
            send_packet_to_proc(entityId, proc, packet);
        }

        // Services are always run every update loop
        if (processManager->processes[i].isService) {
            run_service_procs(entityId, proc);
        }
    }

    // Then, go through IPC proc msg Q and send messages to procs
    ProcMessageQueue* procMessageQueue = (ProcMessageQueue*)g_hash_table_lookup(componentRegistry.procMsgQueues, entityId);
    ProcMessage* message = NULL;
    if (procMessageQueue != NULL) {
        message = proc_msg_queue_read(procMessageQueue);
        if (message != NULL) {
            Process proc = processManager->processes[message->pid];
            if (proc.invocable) {
                send_message_to_proc(entityId, &proc, message);
            }
        }
    }
}

static void send_packet_to_proc(char* entityId, Process* process, Packet* packet) {
    switch (process->program.type) {
        case ProgramTypePing:
            proc_ping_handle_packet(entityId, process, packet);
            break;
        case ProgramTypeLogin:
            proc_login_handle_packet(entityId, process, packet);;
        default:
            break;
    }
}

static void send_message_to_proc(char* entityId, Process* process, ProcMessage* message) {
    switch (process->program.type) {
        case ProgramTypePing:
            proc_ping_handle_message(entityId, process, message);
            break;
        case ProgramTypeScan:
            proc_netscan_handle_message(entityId, process, message);
            break;
        case ProgramTypeLogin:
            proc_login_handle_message(entityId, process, message);
            break;
        default:
            break;
    }
}

static void run_service_procs(char* entityId, Process* process) {
    switch (process->program.type) {
        case PROCESS_TYPE_CHECK_ORIGIN:
            proc_check_origin_handle_service(entityId, process);
            break;
        default:
            break;
    }
}
