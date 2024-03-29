#include <stdio.h>
#include "router.h"
#include "../components/component_registry.h"

Router* entity_router_create_blank() {
    Router* router = calloc(1, sizeof(Router));
    char *entityId = generate_uuid();
    strcpy(router->entityId, entityId);

    char *deviceId = generate_device_id();
    router->device.type = DEVICE_TYPE_ROUTER;
    router->device.owner = DEVICE_OWNER_NOBODY;
    router->device.pwned = DEVICE_NOT_PWNED;
    strncpy(router->device.entityId, entityId, UUID_STR_LEN);
    strncpy(router->device.name, deviceId, DEVICE_NAME_LEN);

    router->position.coord.x = 0;
    router->position.coord.y = 0;

    router->sprite.spriteId = SPRITE_ROUTER;

    strncpy(router->packetBuffer.entityId, entityId, UUID_STR_LEN);
    router->packetBuffer.sendQ = packet_queue_alloc(50);
    router->packetBuffer.recvQ = packet_queue_alloc(50);

    free(entityId);
    free(deviceId);

    return router;
}

char* entity_router_register_components(Router* router) {
    char* entityId = strdup(router->entityId);

    g_hash_table_insert(componentRegistry.devices, entityId, &router->device);
    g_hash_table_insert(componentRegistry.positions, entityId, &router->position);
    g_hash_table_insert(componentRegistry.sprites, entityId, &router->sprite);
    g_hash_table_insert(componentRegistry.routeTable, entityId, &router->routeTable);

    PacketBuffer* packetBuffer = calloc(1, sizeof(PacketBuffer));
    packetBuffer->sendQ = packet_queue_alloc(50);
    packetBuffer->recvQ = packet_queue_alloc(50);
    strcpy(packetBuffer->entityId, entityId);
    g_hash_table_insert(componentRegistry.packetBuffers, entityId, packetBuffer);

    return entityId;
}

/**
 * Tab-separated values of serialized components.
 * Serialized components themselves are semicolon-separated values
 *
 * @param router
 * @return
 */
char* entity_router_serialize(Router router) {
    char buffer[10000] = "";

    char* strDevice = comp_device_serialize(&router.device);
    char* strPosition = comp_position_serialize(&router.position);
    char* strSprite = comp_sprite_serialize(&router.sprite);
    char* strRouteTable = comp_route_table_serialize(&router.routeTable);
    char* strPacketBuffer = comp_packet_buffer_serialize(&router.packetBuffer);

    sprintf(buffer, "%s\t%s\t%s\t%s\t%s\t%s", router.entityId, strDevice, strPosition, strSprite, strRouteTable, strPacketBuffer);

    char* data = calloc(strlen(buffer)+1, sizeof(char));
    strcpy(data, buffer);

    free(strDevice);
    free(strPosition);
    free(strSprite);
    free(strRouteTable);
    free(strPacketBuffer);

    return data;
}

Router entity_router_deserialize(char* data) {
    char** routerData = g_strsplit(data, "\t", 7);

    Router router;
    strcpy(router.entityId, routerData[0]);
    router.device = *(comp_device_deserialize(routerData[1]));
    router.position = *(comp_position_deserialize(routerData[2]));
    router.sprite = *(comp_sprite_deserialize(routerData[3]));
    router.routeTable = *(comp_route_table_deserialize(routerData[4]));
    router.packetBuffer = *(comp_packet_buffer_deserialize(routerData[5]));

    return router;
}
