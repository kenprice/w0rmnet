#ifndef W0RMNET_ROUTING_H
#define W0RMNET_ROUTING_H

/**
 * @param entity_id From which device (addresses are relative to a device)
 * @param address
 * @return entity_id of device, if found, else NULL
 */
char* search_device_by_address(char* entity_id, char* address);

/**
 * Find Device ID of the common ancestor of two entities with Connection
 *
 * @param entityId1
 * @param entityId2
 * @return Device ID of common ancestor
 */
char* network_find_common_ancestor(char* entityId1, char* entityId2);

#endif //W0RMNET_ROUTING_H
