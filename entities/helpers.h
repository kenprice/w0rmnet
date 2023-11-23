/**
 * Helpers for entities, for computations and operations involving multiple component types
 */

#ifndef W0RMNET_HELPERS_H
#define W0RMNET_HELPERS_H

#include <stdbool.h>
#include "../components/packet.h"

bool entity_send_packet_from_entity(char* fromEntity, Packet* packet);

#endif //W0RMNET_HELPERS_H
