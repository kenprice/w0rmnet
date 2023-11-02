#ifndef W0RMNET_WIRE_H
#define W0RMNET_WIRE_H

#include "../utils/uuid.h"
#include "./packet.h"

/**
 * Connects two entities together, bidirectionally
 */
typedef struct {
    char entityA[UUID_STR_LEN];
    char entityB[UUID_STR_LEN];

    PacketQueue sendQtoA;
    PacketQueue sendQtoB;
} Wire;

void register_wire(Wire wire, char* entityId);

char* create_and_register_wire(char* entityA, char* entityB);

Wire* search_wire_by_entity_ids(char* entityId1, char* entityId2);

char* comp_wire_get_other_entity(char* wireEntityId, char* thisEntityId);

#endif //W0RMNET_WIRE_H
