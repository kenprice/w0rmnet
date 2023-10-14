#ifndef W0RMNET_PROCESS_H
#define W0RMNET_PROCESS_H

typedef enum {
    PROCESS_TYPE_PING,
    PROCESS_TYPE_ECHO,
} ProcessType ;

#define PROCESS_ARGS_LEN 100

typedef struct {
    char fingerprint[32]; // Identifies the program being run
    ProcessType type;
    char name[32]; // Name of program

    bool invokable; // Can be invoked by owner of device, invocation is done via message Q ipc
    char args[PROCESS_ARGS_LEN];     // Set if player or something else has invoked, with args
} Process; // Models a running program

typedef struct {
    Process processes[10];
    int num_procs;
    int max_procs;
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
