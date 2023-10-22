#ifndef W0RMNET_PROCESS_H
#define W0RMNET_PROCESS_H

#include "../utils/uuid.h"

#define PROCESS_ARGS_LEN 100
#define PROCESS_STATE_LEN 100

typedef enum {
    PROCESS_TYPE_PING,
    PROCESS_TYPE_ECHO,
    PROCESS_TYPE_SCAN,
    PROCESS_TYPE_LOGIN,
} ProcessType ;

extern const char* ProcessTypeLabel[];

typedef struct {
    ProcessType type;

    bool invocable; // Can be invoked by owner of device, invocation is done via message Q ipc
    char state[PROCESS_STATE_LEN];   // Proces state containing arbitrary bytes.
} Process; // Models a running program

typedef struct {
    Process processes[10];
    int numProcs;
    int maxProcs;
} ProcessManager;


// Message Q as a means of "IPC" for our "Processes"
typedef struct {
    int pid; // "PID" here is index of process_manager.processes
    char* args;
    char fromEntityId[UUID_STR_LEN]; // Source entity ID, empty if none
} ProcMessage;

typedef struct {
    size_t head;
    size_t tail;
    size_t size;
    ProcMessage** messages;
} ProcMessageQueue;

ProcMessage* proc_msg_alloc(int pid, char* args);

ProcMessageQueue proc_msg_queue_alloc(size_t size);

ProcMessage* proc_msg_queue_read(ProcMessageQueue* queue);

int proc_msg_queue_write(ProcMessageQueue* queue, ProcMessage* message);

void register_process_manager(ProcessManager process_manager, char* entity_id);

void register_proc_msg_queue(ProcMessageQueue queue, char* entity_id);

char* comp_process_manager_serialize(ProcessManager* processManager);

ProcessManager* comp_process_manager_deserialize(char* data);

#endif //W0RMNET_PROCESS_H
