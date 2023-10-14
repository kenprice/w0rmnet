#ifndef W0RMNET_PROCESS_H
#define W0RMNET_PROCESS_H

typedef enum {
    PROCESS_TYPE_PING,
    PROCESS_TYPE_ECHO,
} ProcessType ;

typedef struct {
    char fingerprint[32]; // Identifies the program being run
    ProcessType type;
    char name[32]; // Name of program
    bool invokable; // Can be invoked by owner of device
} Process; // Models a running program

typedef struct {
    Process processes[10];
    int num_procs;
    int max_procs;
} ProcessManager;

void register_process_manager(ProcessManager process_manager, char* entity_id);

#endif //W0RMNET_PROCESS_H
