#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glib.h"
#include "component_registry.h"
#include "process.h"

void register_process_manager(ProcessManager process_manager, char* entity_id) {
    ProcessManager* new_process_manager = calloc(1, sizeof(ProcessManager));
    memcpy(new_process_manager, &(process_manager), sizeof(ProcessManager));
    g_hash_table_insert(componentRegistry.processManagers, entity_id, new_process_manager);
}

void register_proc_msg_queue(ProcMessageQueue queue, char* entity_id) {
    ProcMessageQueue* new_proc_msg_q = calloc(1, sizeof(ProcMessageQueue));
    new_proc_msg_q->head = queue.head;
    new_proc_msg_q->tail = queue.tail;
    new_proc_msg_q->size = queue.size;
    new_proc_msg_q->messages = queue.messages;
    g_hash_table_insert(componentRegistry.procMsgQueues, entity_id, new_proc_msg_q);
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

char* comp_process_manager_serialize(ProcessManager* processManager) {
    char procsBuffer[10000] = "";
    for (int i = 0; i < processManager->numProcs; i++) {
        char processBuffer[1000] = "";
        Process proc = processManager->processes[i];
        sprintf(processBuffer, "%d$%d$%s", proc.program.type, proc.invocable, proc.state);
        strcat(procsBuffer, processBuffer);
        strcat(procsBuffer, "|");
    }
    procsBuffer[strlen(procsBuffer) - 1] = '\0';

    char procMgrBuffer[10000];
    sprintf(procMgrBuffer, "%s;%d;%d", procsBuffer, processManager->numProcs, processManager->maxProcs);

    char* data = calloc(strlen(procMgrBuffer)+1, sizeof(char));
    strcpy(data, procMgrBuffer);
    return data;
}

ProcessManager* comp_process_manager_deserialize(char* data) {
    ProcessManager* processManager = calloc(1, sizeof(ProcessManager));

    char** splitData = g_strsplit(data, ";", 3);

    processManager->numProcs = atoi(splitData[1]);
    processManager->maxProcs = atoi(splitData[2]);

    char** procs = g_strsplit(splitData[0], "|", processManager->numProcs);
    for (int i = 0; i < processManager->numProcs; i++) {
        char** proc = g_strsplit(procs[i], "$", 3);
        processManager->processes[i].program.type = atoi(proc[0]);
        processManager->processes[i].invocable = atoi(proc[1]);
        strcpy(processManager->processes[i].state, proc[2]);
        g_strfreev(proc);
    }

    g_strfreev(splitData);
    g_strfreev(procs);

    return processManager;
}
