#ifndef W0RMNET_NETSCAN_RUNNER_H
#define W0RMNET_NETSCAN_RUNNER_H

#include "../../components/components.h"

/**
 * @param entity_id entity on which the process is running
 * @param process the process
 * @param packet received packet
 */
void proc_netscan_handle_packet(char* entity_id, Process* process, Packet* packet);

/**
 * @param entityId entity on which the process is running
 * @param process the process
 * @param message received message
 */
void proc_netscan_handle_message(char* entityId, Process* process, ProcMessage* message);

#endif //W0RMNET_NETSCAN_RUNNER_H
