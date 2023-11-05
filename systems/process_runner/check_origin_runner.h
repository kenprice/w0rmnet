#ifndef W0RMNET_CHECK_ORIGIN_RUNNER_H
#define W0RMNET_CHECK_ORIGIN_RUNNER_H

#include "../../components/components.h"

/**
 * If from address of packet is not in whitelist, send failure to FORWARD PACKET prog (if exists)
 * State: <address for A queue>,...;<address for B queue>,...
 * Whitelist for A queue, whitelist for B queue
 *
 * @param entityId entity on which the process is running
 * @param process the process
 * @param packet received packet
 */
void proc_check_origin_handle_service(char* entityId, Process* process);

#endif //W0RMNET_CHECK_ORIGIN_RUNNER_H
