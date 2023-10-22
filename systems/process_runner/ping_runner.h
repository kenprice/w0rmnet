#ifndef W0RMNET_PING_RUNNER_H
#define W0RMNET_PING_RUNNER_H

#include "../../components/components.h"

/**
 * @param entityId entity on which the process is running
 * @param process the process
 * @param packet received packet
 */
void proc_ping_handle_packet(char* entityId, Process* process, Packet* packet);

/**
 * @param entityId entity on which the process is running
 * @param process the process
 * @param message received message
 */
void proc_ping_handle_message(char* entityId, Process* process, ProcMessage* message);

#endif //W0RMNET_PING_RUNNER_H
