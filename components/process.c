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

void register_proc_msg_queue(ProcMessageQueue queue, char* entity_id) {
    ProcMessageQueue* new_proc_msg_q = calloc(1, sizeof(ProcMessageQueue));
    memcpy(new_proc_msg_q, &(queue), sizeof(ProcMessageQueue));
    g_hash_table_insert(component_registry.proc_msg_queues, entity_id, new_proc_msg_q);
}

ProcMessage* proc_msg_alloc(int pid, char* args) {
    ProcMessage* proc_msg = calloc(1, sizeof(ProcMessage));
    proc_msg->pid = pid;
    proc_msg->args = calloc(1, sizeof(char) * strlen(args) + 1);
    strcpy(proc_msg->args, args);
    return proc_msg;
}

ProcMessageQueue proc_msg_queue_alloc(size_t size) {
    ProcMessageQueue queue;
    queue.size = size;
    queue.tail = 0;
    queue.head = 0;
    queue.messages = calloc(size, sizeof(ProcMessage *));
    return queue;
}

ProcMessage* proc_msg_queue_read(ProcMessageQueue* queue) {
    if (queue->tail == queue->head) {
        return NULL;
    }
    void* handle = queue->messages[queue->tail];
    queue->messages[queue->tail] = NULL;
    queue->tail = (queue->tail + 1) % queue->size;
    return handle;
}

int proc_msg_queue_write(ProcMessageQueue* queue, ProcMessage* message) {
    if (((queue->head + 1) % queue->size) == queue->tail) {
        return -1;
    }
    queue->messages[queue->head] = message;
    queue->head = (queue->head + 1) % queue->size;
    return 0;
}
