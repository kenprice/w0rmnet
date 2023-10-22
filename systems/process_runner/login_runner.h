#ifndef W0RMNET_LOGIN_RUNNER_H
#define W0RMNET_LOGIN_RUNNER_H

#include "../../components/components.h"

/**
 * If player uses correct credentials, grants ownership
 *
 * @param entity_id entity on which the process is running
 * @param process the process
 * @param packet received packet
 */
void proc_login_handle_packet(char* entity_id, Process* process, Packet* packet);

/**
 * Sends remote login, message is "<address>:<username>:<password>"
 *
 * @param entityId entity on which the process is running
 * @param process the process
 * @param message received message
 */
void proc_login_handle_message(char* entityId, Process* process, ProcMessage* message);


#endif //W0RMNET_LOGIN_RUNNER_H
