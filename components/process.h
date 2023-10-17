#ifndef W0RMNET_PROCESS_H
#define W0RMNET_PROCESS_H

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
    char fingerprint[32]; // Identifies the program being run
    ProcessType type;
    char name[32]; // Name of program

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
    // TODO: Track who sent?
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

#endif //W0RMNET_PROCESS_H
