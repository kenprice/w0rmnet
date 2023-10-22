#ifndef W0RMNET_ROUTING_H
#define W0RMNET_ROUTING_H

/**
 * @param entityId From which device (addresses are relative to a device)
 * @param address
 * @return entityId of device, if found, else NULL
 */
char* search_device_by_relative_address(char* entityId, char* address);

/**
 * Convert a full address to a relative address, relative to device specified by fromEntityId
 *
 * @param fromEntityId
 * @param targetAddress
 * @return Shortened, relative address
 */
char* convert_full_address_to_relative_address(char* fromEntityId, char* targetAddress);

/**
 * Find Device ID of the common ancestor of two entities with Connection
 *
 * @param entityId1
 * @param entityId2
 * @return Device ID of common ancestor
 */
char* network_find_common_ancestor(char* entityId1, char* entityId2);

#endif //W0RMNET_ROUTING_H
